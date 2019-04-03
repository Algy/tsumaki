#include <fstream>
#include "unix.hpp"

namespace tsumaki::ipc {
    const std::string pid_file_path = "/var/run/tsumaki.pid";
    const std::string base_file_path = "/usr/local/share/tsumaki";

    bool UnixIPC::check_process() {
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

    bool UnixIPC::spawn_process() {
        std::string bin_path = base_file_path;
        bin_path += '/';
        bin_path += bin_version;
        bin_path += "/tsumaki";

        // now spawn a process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to spawn a tsumaki process");
            return false;
        } else if (pid == 0) {
            std::vector<std::string> argv { bin_path, "run" };
            argv.push_back("--host");
            argv.push_back(host);
            argv.push_back("--port");
            argv.push_back(std::to_string(port));

            const char **argvp = new const char*[argv.size() + 1];
            for (std::size_t i = 0; i < argv.size(); i++) {
                argvp[i] = argv[i].c_str();
            }
            argvp[argv.size()] = nullptr;
            execv(bin_path.c_str(), (char**)argvp);
            perror("Failed to spawn a tsumaki process");
            delete [] argvp;
            exit(127);
        } else {
            // parent
            std::ofstream file(pid_file_path);
            file << (int)pid;
        }
        return true;
    }

    void UnixIPC::terminate_process() {
        int pid_n;
        std::ifstream pid_file(pid_file_path);
        if (pid_file.fail()) return;
        pid_file >> pid_n;
        if (pid_n >= 1) {
            pid_t pid = (pid_t)pid_n;
            kill(pid, SIGTERM);
            std::remove(pid_file_path.c_str());
        }
    }
}
