#include <obs-module.h>
#include <util/circlebuf.h>
#include <fstream>
#include <memory>
#include "tsumaki-filter.hpp"
#include "ipc-error.hpp"

#include "protobuf/Heartbeat.pb.h"


namespace tsumaki {
    TsumakiFilter::TsumakiFilter() : OBSFilter() {
        curr_ipc = unique_ptr<ipc::IPC>(new AvailableIPC("127.0.0.1", 1125));
    }
    void TsumakiFilter::run_once() {
        ipc::IPC::init_ipc_system();
    }
    TsumakiFilter::~TsumakiFilter() {
    }


    void TsumakiFilter::init() {

        if (!curr_ipc->check_process()) {
            curr_ipc->spawn_process();
        }

        try {
            curr_ipc->sleep(5000);
            std::shared_ptr<HeartbeatRequest> req { new HeartbeatRequest() };
            req->set_hello("HELLO!");
            auto result = curr_ipc->request_sync(req);
            if (result.success) {
                auto resp = static_cast<HeartbeatResponse*>(result.message.get());
                info << "Recv " << resp->hello() << info.endl;
            } else {
                error << result.error_code << " " << result.error_code << error.endl;
            }
        } catch (ipc::IPCError &err) {
            error << err.what() << error.endl;
        }
    }

    void TsumakiFilter::destroy() {
    }

    void TsumakiFilter::update_settings(obs_data_t *settings) {
        this->interval = (uint64_t)obs_data_get_int(settings, "delay_ms") * 1000000ULL;
    }

    void TsumakiFilter::get_properties(obs_properties_t *props) {
        obs_properties_add_int(props, "delay_ms", T("DelayMS"), 0, 20000, 1);
    }

    void TsumakiFilter::detach(obs_source_t *parent) { (void)parent; }

    unique_ptr<Frame> TsumakiFilter::frame_update(unique_ptr<Frame> frame) {
        auto rgba = frame->get_rgba_image();
        // blog(LOG_INFO, "SIZE: %d", (int)rgba->get_size());
        for (int i = 0; i < rgba->get_size(); i++) {
            // rgba->data[i] = 255 - rgba->data[i];
        }
        unique_ptr<Frame> new_frame { new RGBAFrame(rgba) };
        return new_frame;
    }
};

