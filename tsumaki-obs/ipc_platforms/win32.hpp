#include "../ipc.hpp"


namespace tsumaki {
    class Win32IPC : public IPC {
        using IPC::IPC;
        virtual bool check_process();
        virtual bool spawn_process();
        virtual void terminate_process();
    };
}
