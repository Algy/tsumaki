#include "ipc-connection.hpp"
#include "ipc-error.hpp"

namespace tsumaki::ipc {
    IPCConnection::~IPCConnection() {
        close();
    }

    void IPCConnection::ensure_connection() {
        if (psocket == nullptr) {
            psocket = std::unique_ptr<net::socket>(new net::socket(net::af::inet, net::sock::stream));
            if (psocket->connect(net::endpoint(host, port)) == -1) {
                throw IPCConnectionError(std::strerror(errno));
            }
        }
    }

    void IPCConnection::write_all(const std::string& content) {
        if (psocket == nullptr) {
            throw IPCError("Not connected yet");
        }

        int num_written = 0;
        const int length = content.size();
        const char *source = content.c_str();

        while (num_written < length) {
            const int num_to_write = std::min(block_size, length - num_written);
            int wrt = psocket->send(source + num_written, num_to_write);
            if (wrt < 0) {
                throw IPCConnectionClosedError(std::strerror(errno));
            }
            num_written += wrt;
        }
    }

    std::string IPCConnection::read_all(int length) {
        if (psocket == nullptr) {
            throw IPCError("Not connected yet");
        }

        std::string result;
        result.reserve(length);

        std::unique_ptr<char> buffer{ new char[std::min(block_size, length)] };
        int num_read = 0;
        while (num_read < length) {
            const int num_to_read = std::min(block_size, length - num_read);
            int rd = psocket->recv(buffer.get(), num_to_read);
            if (rd < 0) {
                throw IPCConnectionClosedError("Connection closed being interrupted: " + std::string(std::strerror(errno)));
            } else if (rd == 0) {
                throw IPCConnectionClosedError("Connection unexpectedly closed. Expected" + std::to_string(length) + "bytes, got " + std::to_string(num_read));
            }
            result.append(buffer.get(), rd);
            num_read += rd;
        }
        return result;
    }

    void IPCConnection::close() {
        if (psocket != nullptr) {
            psocket->close();
            psocket = nullptr;
        }
    }
}
