#include <algorithm>
#include <fstream>
#include <string>
#include <wstring>
#include <sys/types.h>
#include <signal.h>
#include <vector>

#include "ipc.hpp"
namespace tsumaki::ipc {
    static std::string get_string () {
    }

    const std::string pid_file_path = "/var/run/tsumaki.pid";
    const std::string base_file_path = "/usr/local/share/tsumaki";
    const std::string bin_version = "0.0.1";

    void IPC::init_ipc_system() {
        net::init();
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
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to spawn a tsumaki process");
            return false;
        } else if (pid == 0) {
            std::string bin_path = base_file_path;
            bin_path += '/';
            bin_path += bin_version;
            bin_path += "/bin/tsumaki";
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
            exit(127);
        } else {
            // parent
            std::string path = "/var/run/tsumaki.pid";
            std::ofstream file(path);
            file << pid;
        }
        return true;
    }

    void terminate_process() {
        int pid_n;
        std::ifstream pid_file(pid_file_path);
        if (pid_file.fail()) return;
        pid_file >> pid_n;
        if (pid_n >= 1) {
            pid_t pid = (pid_t)pid_n;
            kill(pid, SIGTERM);
        }
    }
    RPCResult request_sync(const ::google::protobuf::Message &message, std::function<void(RPCResult &result)> callback, int timeout = 0) {
        RPCResult result;
        return result;
    }
}

