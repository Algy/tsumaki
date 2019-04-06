#pragma once
#include <thread>
#include <memory>
#include "ipc.hpp"
#include "obs-filter.hpp"
#include "thread-util.hpp"

namespace tsumaki {
    class ApiThread : public OBSLoggable {
    private:
        std::unique_ptr<ipc::IPC> curr_ipc;
        bool ready = false;
        bool impaired = false;

        bool run_flag = false;

        threadutil::UniquePtrQueue<Frame> input_queue;
        threadutil::UniquePtrQueue<Frame> output_queue;
    public:
        ApiThread();
    public:
        bool is_ready() { return ready; };
        bool is_impared() { return impaired; };
        bool is_preparing() { return !ready && !impaired; };
        void start_thread(std::shared_ptr<ApiThread> this_ref);
        void stop_thread();
        void run();

        void put_frame(std::unique_ptr<Frame> frame);
        std::unique_ptr<Frame> get_frame();
        static void init_once();
        virtual const char* get_scope_name() const { return "Tsumaki-ApiThread"; };

    };
};
