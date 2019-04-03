#pragma once
#include <string>
#include <memory>
#include "deps/xsocket.hpp"

namespace tsumaki::ipc {
    class IPCConnection {
    private:
        const std::string host;
        const int port;
        std::shared_ptr<net::socket> psocket;
        const int block_size;
    public:
        IPCConnection(std::string host, int port, int block_size = 8388608): host(host), port(port), block_size(block_size) {};
        ~IPCConnection();
        void ensure_connection();
        void write_all(const std::string& content);
        std::string read_all(int length);
        void close();
    };
}

