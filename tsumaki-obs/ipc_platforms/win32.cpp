#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "win32.hpp"

namespace tsumaki {
	static void raise_winerror(std::string tag) {
		int ret = GetLastError();
		throw IPCError(tag + " failed, error = " + std::to_string(ret));
	}

	static bool is_process_running(DWORD processID) {
		if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID)) {
			DWORD exitCodeOut;
			// GetExitCodeProcess returns zero on failure
			if (GetExitCodeProcess(process, &exitCodeOut) != 0)
			{
				// Return if the process is still active
				return exitCodeOut == STILL_ACTIVE;
			}
		}
		return false;
	}

	static std::wstring ascii2ws(const std::string &str) {
		std::wstring ws;
		std::for_each(str.cbegin(), str.cend(), [&ws](char c) { ws += (wchar_t)c; });
		return ws;
	}

	void dummy_function() {}
	static std::wstring get_current_module_dir() {
		TCHAR path[MAX_PATH + 1];
		std::fill(path, path + MAX_PATH + 1, 0);
		HMODULE hm = NULL;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR)&dummy_function, &hm) == 0) {
			raise_winerror("GetModuleHandle");
		}
		if (GetModuleFileName(hm, path, MAX_PATH) == 0) {
			raise_winerror("GetModuleFileName");
		}
		std::wstring module_path = path;
		size_t last_sep_idx = module_path.find_last_of(L'\\');
		return module_path.substr(0, last_sep_idx);
	}

	bool Win32IPC::check_process() {
		int pid = find_pid();
		if (pid != -1)
			return is_process_running(pid);
		else
			return false;
	}

	bool Win32IPC::spawn_process() {
		std::wstring ws_bin_version = ascii2ws(bin_version);
		std::wstring module_name = get_current_module_dir() + L"\\tsumaki\\" + ws_bin_version + L"\\tsumaki.exe";

		std::wstringstream cmd_stream;
		cmd_stream << "run " << "--port" << port << " " << "--host" << ascii2ws(host);
		std::wstring cmd_line = cmd_stream.str();

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		BOOL success = CreateProcess(
			module_name.c_str(),
			(LPWSTR)cmd_line.c_str(),
			NULL, // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory
			&si,            // Pointer to STARTUPINFO structure
			&pi           // Pointer to PROCESS_INFORMATION structure
		);

		if (success == FALSE) {
			return false;
		}
		pid = pi.dwProcessId;
		std::wofstream pid_file(get_pid_file_path());
		pid_file << pid;
		return true;
	}

	void Win32IPC::terminate_process() {
		int pid = find_pid();
		if (pid == -1) return;
		HANDLE handy;
		handy = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
		TerminateProcess(handy, 0);
	}

	int Win32IPC::find_pid() {
		if (pid != -1)
			return pid;
		std::wstring pid_path = get_pid_file_path();
		int pid_n = -1;
		std::wifstream pid_file(pid_path);
		if (pid_file.bad()) {
			return -1;
		}
		pid_file >> pid_n;
		if (pid_n >= 1) {
			return pid_n;
		}
		return -1;
	}

	std::wstring Win32IPC::get_pid_file_path() {
		TCHAR path[MAX_PATH + 1];
		std::fill(path, path + MAX_PATH + 1, 0);
		DWORD path_length = GetTempPath(MAX_PATH, path);
		if (path_length == 0) {
			raise_winerror("GetTempPath");
		}
		return std::wstring(path) + L"\\" + L"tsumaki.pid";
	}

	void Win32IPC::sleep(unsigned int milliseconds) {
		Sleep(milliseconds);
	}
}
