#include "tsumaki-api-thread.hpp"
#include "platform-def.hpp"
#include "protobuf/Heartbeat.pb.h"
#include <algorithm>

#define MAX_ATTEMPTS 10

namespace tsumaki {
    void ApiThread::init_once() {
        IPC::init_ipc_system();
    }

    ApiThread::ApiThread() : OBSLoggable(), input_queue(1), output_queue(1) {
        curr_ipc = std::unique_ptr<IPC>(new AvailableIPC("127.0.0.1", 1125));
    }

    void ApiThread::start_thread(std::shared_ptr<ApiThread> this_ref) {
        run_flag = true;
        std::thread([this_ref] { this_ref->run(); }).detach();
    }

    void ApiThread::stop_thread() {
        if (run_flag) {
            run_flag = false;
        }
    }

    // Run in background thread
    void ApiThread::run() {
        bool heartbeat_ok = false;
        for (int i = 0; i < MAX_ATTEMPTS; i++) {
            if (!curr_ipc->check_process()) {
                curr_ipc->spawn_process();
                info << "Spawning worker process" << info.endl;
            }
            curr_ipc->sleep(1000);
            std::shared_ptr<HeartbeatRequest> req { new HeartbeatRequest() };
            req->set_hello("Hi");
            try {
                auto result = curr_ipc->request_sync(req);
                auto resp = result.get_response<HeartbeatResponse>();
                if (!result.success || resp.hello() != "Hi") {
                    this->impaired = true;
                    return;
                }

                result = curr_ipc->request_sync(req);
                resp = result.get_response<HeartbeatResponse>();
                if (!result.success || resp.hello() != "Hi") {
                    this->impaired = true;
                    return;
                }
                heartbeat_ok = true;
                break;
            } catch (IPCError &err) {
                error << "Retrying due to ipc error: " << err.what() << error.endl;
            }
        }
        if (!heartbeat_ok) {
            info << "Failed to connect to the worker process(max attempts=" << MAX_ATTEMPTS << "). Something's gone wrong :(" << info.endl;
            this->impaired = true;
            return;
        }
        info << "Connected to the worker process!" << info.endl;

        while (run_flag) {
            std::unique_ptr<Frame> frame = input_queue.get(1000);
            if (frame == nullptr) {
                continue;
            }

            auto rgba = frame->get_rgba_image();
            try {
                std::shared_ptr<DetectPersonRequest> req { new DetectPersonRequest() };

                auto p_image = req->mutable_image();
                p_image->set_width(rgba->width);
                p_image->set_height(rgba->height);
                p_image->set_data(rgba->data, rgba->get_size());
                req->set_base_dimension(512);
                auto p_neural_param = req->mutable_neural_param();
                p_neural_param->set_branch("incubator");
                p_neural_param->set_name("mobilenetv2");
                p_neural_param->set_version("0.0.1");
                p_neural_param->set_dimension(256);
                auto fn = [] { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); };

                auto t1 = fn();
                auto result = curr_ipc->request_sync(req);
                auto t2 = fn();


                if (result.success) {
                    // auto resp = result.get_response<DetectPersonResponse>();
                    auto resp = std::dynamic_pointer_cast<DetectPersonResponse>(result.message);
                    last_mask_image = rgba;
                    int mask_width = resp->mask().width();
                    int mask_height = resp->mask().height();
                    const std::string& data = resp->mask().data();
                    last_mask = std::shared_ptr<ConvertedMaskImage>(new ConvertedMaskImage(mask_width, mask_height));
                    std::copy(data.c_str(), data.c_str() + last_mask->get_size(), last_mask->data);
                    last_mask_response = resp;
                } else {
                    error << result.error_message << error.endl;
                }
            } catch (IPCError &err) {
                error << err.what() << error.endl;
            }
            //output_queue.put_replace(std::move(frame));
        }
    }

    void ApiThread::put_frame(std::unique_ptr<Frame> frame) {
        input_queue.put_replace(std::move(frame));
    }

    std::unique_ptr<Frame> ApiThread::get_frame() {
        return output_queue.get(1000);
    }
};
