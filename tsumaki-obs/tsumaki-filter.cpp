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
        api_thread->start_thread(api_thread);
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
        int width = frame->get_width();
        int height = frame->get_height();
        // api_thread->put_frame(std::move(frame));
        std::shared_ptr<ConvertedRGBAImage> copied_img { new ConvertedRGBAImage(*frame->get_rgba_image()) };
        api_thread->put_frame(std::unique_ptr<Frame>(new RGBAFrame(copied_img)));

        if (api_thread->last_mask_response != nullptr) {
            auto acquired_resp = api_thread->last_mask_response;
            if (acquired_resp != nullptr) {
                auto &mask = acquired_resp->mask();
                int mask_width = mask.width();
                int mask_height = mask.height();
                const std::string& data = mask.data();

                if (mask_width == width && mask_height == height) {
                    auto frame_rgba = frame->get_rgba_image();
                    for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            if ((uint8_t)data[i * width + j] < 128) {
                                uint8_t *base_ptr = &frame_rgba->data[i * (width * 4) + j * 4];
                                base_ptr[0] = 0;
                                base_ptr[1] = 255;
                                base_ptr[2] = 0;
                            }
                        }
                    }
                    return std::unique_ptr<Frame>(new RGBAFrame(frame_rgba));
                }
            }
        }
        return frame;
    }
};

