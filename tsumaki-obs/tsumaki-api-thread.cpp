#include "tsumaki-api-thread.hpp"
#include "platform-def.hpp"
#include "protobuf/Heartbeat.pb.h"
#define MAX_ATTEMPTS 10

/*
 *
 *
 */
namespace tsumaki {
    void ApiThread::init_once() {
        ipc::IPC::init_ipc_system();
    }

    ApiThread::ApiThread() : OBSLoggable() {
        curr_ipc = std::unique_ptr<ipc::IPC>(new AvailableIPC("127.0.0.1", 1125));
    }

    void ApiThread::start_thread() {
        run_flag = true;
        std::shared_ptr<ApiThread> this_ref{ this };
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
                if (resp.hello() != "Hi") {
                    this->impaired = true;
                    return;
                }
                heartbeat_ok = true;
                break;
            } catch (ipc::IPCError &err) {
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
            curr_ipc->sleep(1000);
            info << "HI" << info.endl;
        }
    }
};
