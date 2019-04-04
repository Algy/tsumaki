#include "../ipc.hpp"


namespace tsumaki::ipc {
    class UnixIPC : public IPC {
        using IPC::IPC;
        virtual bool check_process();
        virtual bool spawn_process();
        virtual void terminate_process();
        virtual void sleep(unsigned int milliseconds);
    };
}