#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <sys/types.h>
#include <signal.h>
#include <vector>
#include <thread>
#include "ipc.hpp"

namespace tsumaki::ipc {
    const std::string pid_file_path = "/var/run/tsumaki.pid";
    const std::string base_file_path = "/usr/local/share/tsumaki";
    const std::string bin_version = "0.0.1";

    IPC::~IPC() {
        base_conn.close();
    }

    IPCConnection IPC::generate_connection() {
        return IPCConnection(host, port);
    }
    void IPC::init_ipc_system() {
        net::init();
        signal(SIGPIPE, SIG_IGN);
    }

    bool IPC::check_process() {
        int pid_n = -1;
        std::ifstream pid_file(pid_file_path);
        if (pid_file.fail()) return false;
        pid_file >> pid_n;
        if (pid_n >= 1) {
            pid_t pid = (pid_t)pid_n;
            return kill(pid, 0) == 0;
        }
        return false;
    }

    bool IPC::spawn_process() {
        std::string bin_path = base_file_path;
        bin_path += '/';
        bin_path += bin_version;
        bin_path += "/bin/tsumaki";
        {
            std::ifstream binfile("logs.txt");
            if (binfile.fail()) {
                return false;
            }
        }

        // now spawn a process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to spawn a tsumaki process");
            return false;
        } else if (pid == 0) {
            std::vector<std::string> argv { bin_path, "run" };

            if (ipc_type.use_tcp) {
                auto port_str = std::to_string(ipc_type.port);
                argv.push_back("--host");
                argv.push_back(ipc_type.host);
                argv.push_back("--port");
                argv.push_back(std::to_string(ipc_type.port));
            } else {
                argv.push_back("--socket");
                argv.push_back(ipc_type.unix_socket);
            }

            char **argvp = new char*[argv.size()];
            for (int i = 0; i < argv.size(); i++) {
                argvp[i] = argv[i].c_str();
            }
            execv(bin_path, argvp);
            perror("Failed to create process");
            delete [] argvp;
            exit(127);
        } else {
            // parent
            std::ofstream file(pid_file_path);
            file << (int)pid;
        }
        return true;
    }

    void IPC::terminate_process() {
        int pid_n;
        std::ifstream pid_file(pid_file_path);
        if (pid_file.fail()) return;
        pid_file >> pid_n;
        if (pid_n >= 1) {
            pid_t pid = (pid_t)pid_n;
            kill(pid, SIGTERM);
            std::remove(pid_file_path);
        }
    }


    static RPCResult do_request(IPCConnection &conn, const Message &message) {
        conn.ensure_connection();
        IPCFrameChannel channel(conn);

        IPCFrame request_frame(IPCFrame::RequestType, &message);
        channel.send(request_frame);
        IPCFrame resp_frame = channel.receive();

        RPCResult result;
        if (resp_frame.is_error()) {
            ErrorResponse& message = static_cast<ErrorResponse&>(resp_frame.get_message());
            result.success = false;
            result.error_code = message.code;
            result.error_message = message.msg;
        } else {
            result.success = true;
            result.message = resp_frame.message;
        }
        return result;
    }

    static RPCResult request_sync(const Message &connection) {
        return request(base_conn, message);
    }

    void request_async(
            std::shared_ptr<Message> message,
            std::function<void(RPCResult &result)> callback,
            std::function<void(IPCError &exc)> error = nullptr) {
        std::shared_ptr<IPCConnection> conn { new IPCConnection(generate_connection()) };
        std::thread([conn, message, callback, error]() {
            try {
                RPCResult rpc_result = do_request(*conn, message);
                callback(rpc_result);
            } catch (IPCError &exc) {
                if (error != nullptr) {
                    error(exc);
                }
                throw exc;
            }
        });
    }
}

