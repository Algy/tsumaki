#pragma once
#include <thread>
#include <memory>
#include "ipc.hpp"
#include "obs-filter.hpp"

namespace tsumaki {
    class ApiThread : public OBSLoggable {
    private:
        std::unique_ptr<ipc::IPC> curr_ipc;
        bool ready = false;
        bool impaired = false;

        bool run_flag = false;
    public:
        ApiThread();
    public:
        bool is_ready() { return ready; };
        bool is_impared() { return impaired; };
        bool is_preparing() { return !ready && !impaired; };
        void start_thread();
        void stop_thread();
        void run();
        static void init_once();
        virtual const char* get_scope_name() const { return "Tsumaki-ApiThread"; };

    private:
        void spawn_and_check();
    };
};
