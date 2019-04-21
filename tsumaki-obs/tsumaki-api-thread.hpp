#pragma once
#include <thread>
#include <memory>
#include "ipc.hpp"
#include "obs-filter.hpp"
#include "thread-util.hpp"
#include "protobuf/DetectPerson.pb.h"

namespace tsumaki {
    class ApiThread : public OBSLoggable {
    private:
        std::unique_ptr<IPC> curr_ipc;
        bool ready = false;
        bool impaired = false;

        bool run_flag = false;


        UniquePtrQueue<Frame> input_queue;
        UniquePtrQueue<Frame> output_queue;
    public:
        ApiThread();
    public:
        std::shared_ptr<DetectPersonResponse> last_mask_response;
        std::shared_ptr<ConvertedRGBAImage> last_mask_image;
        std::shared_ptr<ConvertedMaskImage> last_mask;
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
