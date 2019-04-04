#include <obs-module.h>
#include <util/circlebuf.h>
#include <fstream>
#include <memory>
#include "tsumaki-filter.hpp"
#include "tsumaki-api-thread.hpp"
#include "ipc-error.hpp"

#include "protobuf/Heartbeat.pb.h"


namespace tsumaki {
    TsumakiFilter::TsumakiFilter() : OBSFilter() {
    }

    void TsumakiFilter::run_once() {
        ApiThread::init_once();
    }

    TsumakiFilter::~TsumakiFilter() {
    }


    void TsumakiFilter::init() {
        api_thread = std::shared_ptr<ApiThread>(new ApiThread());
        api_thread->start_thread();
    }

    void TsumakiFilter::destroy() {
        api_thread->stop_thread();
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

