#include "../ipc.hpp"


namespace tsumaki {
    class Win32IPC : public IPC {
    private:
        DWORD pid = -1;
    public:
        using IPC::IPC;
        virtual bool check_process();
        virtual bool spawn_process();
        virtual void terminate_process();
	virtual void sleep(unsigned int milliseconds);

    private:
	    int find_pid();
	    std::wstring get_pid_file_path();
    };
}
