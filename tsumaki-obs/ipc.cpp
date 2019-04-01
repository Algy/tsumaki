#include <algorithm>
#include <fstream>
#include <string>
#include <wstring>

#include "ipc.hpp"
#ifndef _WIN32
// unix
#define _GNU_SOURCE
#include <dlfcn.h>

#include <unistd.h>
#else
// windows
#include <windows.h>
#endif

namespace tsumaki::ipc {
    static std::string wide_string_to_string(std::wstring wstr)
    {
        if (wstr.empty())
        {
            return std::string();
        }
#if defined WIN32
        int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], wstr.size(), NULL, 0, NULL, NULL);
        std::string ret = std::string(size, 0);
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], wstr.size(), &ret[0], size, NULL, NULL);
#else
        size_t size = 0;
        _locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
        errno_t err = _wcstombs_s_l(&size, NULL, 0, &wstr[0], _TRUNCATE, lc);
        std::string ret = std::string(size, 0);
        err = _wcstombs_s_l(&size, &ret[0], size, &wstr[0], _TRUNCATE, lc);
        _free_locale(lc);
        ret.resize(size - 1);
#endif
        return ret;
    }


    void IPC::check_process() {
    }

    bool IPC::spawn_process() {
#ifndef _WIN32
        int pid = fork();
        if (pid < 0) {
            return false;
        } else if (pid == 0) {
            if (use_tcp) {
                auto port_str = std::to_string(port);
                execlp(
                    "python", "-m", "tsumaki", "run",
                    "--host", host.c_str(),
                    "--port", port_str.c_str(),
                    nullptr
                );
            } else {
                execlp(
                    "python", "-m", "tsumaki", "run",
                    "--socket", unix_socket.c_str(),
                    nullptr
                );
            }
            perror("Failed to create process");
            exit(127);
            // Unreachable
        } else {
            // parent
            std::string path = wide_string_to_string(get_pid_file_name());
            std::ofstream file(path);
            file << pid;
        }
        return true;
#else
        STARTUPINFO info={sizeof(info)};
        PROCESS_INFORMATION processInfo;
        if (CreateProcess(path, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        {
                WaitForSingleObject(processInfo.hProcess, INFINITE);
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
        }
        return true;
#endif
    }

    std::wstring get_pid_file_name() {
#ifndef _WIN32
        return L"/tmp/tsumaki.pid";
#else
        wchar_t buf[MAX_PATH + 1];
        std::fill_n(buf, MAX_PATH, 0);
        GetTempPathW(MAX_PATH, buf);

        std::wstring path(buf);
        path += L"\\tsumaki.pid";
        return path;
#endif
    }

};
 
