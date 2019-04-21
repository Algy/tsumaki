#include <obs-module.h>
#include <util/circlebuf.h>
#include <fstream>
#include <memory>
#include <algorithm>
#include <vector>
#include "tsumaki-filter.hpp"
#include "tsumaki-api-thread.hpp"
#include "ipc-error.hpp"

#include "protobuf/Heartbeat.pb.h"

#include "deps/fast-slic/fast-slic.h"
#include "deps/fast-slic/fast-slic-avx2.h"
#include "deps/fast-slic/simple-crf.hpp"


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

        std::shared_ptr<ConvertedRGBAImage> img { new ConvertedRGBAImage(frame->get_rgba_image()->resize_bilinear(256, 144)) };
        api_thread->put_frame(std::move(unique_ptr<Frame> { new RGBAFrame(img) }));

        auto acquired_resp = api_thread->last_mask_response;

        auto frame_rgba = frame->get_rgba_image();
        ConvertedRGBImage frame_rgb = *frame_rgba;
        ConvertedMaskImage frame_result_mask(width, height);

        if (acquired_resp != nullptr) {
            auto mask = api_thread->last_mask;
            auto mask_image = api_thread->last_mask_image;

            ConvertedMaskImage mask_resized = mask->resize_bilinear(width, height);
            Cluster clusters[800];
            uint8_t cluster_densities[800];
            std::vector<uint32_t> assignment(height * width);
            fast_slic_initialize_clusters_avx2(height, width, 800, frame_rgb.data, clusters);
            fast_slic_iterate_avx2(height, width, 800, 1, 6, 10, frame_rgb.data, clusters, &assignment[0]);
            fast_slic_get_mask_density(height, width, 800, clusters, &assignment[0], mask_resized.data, cluster_densities);
            Connectivity *conn = fast_slic_knn_connectivity(height, width, 800, clusters, 20);
            SimpleCRF crf(2, 800);
            auto &crf_frame = crf.push_frame();
            crf_frame.set_connectivity(conn);
            fast_slic_free_connectivity(conn);
            crf_frame.set_clusters(clusters);
            std::vector<float> proba(800 * 2);
            std::vector<float> result_proba(800 * 2);
            for (int i = 0; i < 800; i++) {
                float p = 0.2f + 0.7f * cluster_densities[i] / 255.0f;
                proba[i] = 1 - p;
                proba[800 + i] = p;
            }
            crf_frame.set_proba(&proba[0]);
            crf.params.spatial_w = 1;
            crf.params.spatial_sxy = 40;
            crf.params.spatial_srgb = 40;
            crf.initialize();
            crf.inference(3);
            crf_frame.get_inferred(&result_proba[0]);
            std::vector<uint8_t> result_densities(800);
            std::transform(result_proba.begin() + 800, result_proba.end(),
                    result_densities.begin(),
                    [](float p) { return (uint8_t)(p * 255); });
            fast_slic_cluster_density_to_mask(height, width, 800, clusters, &assignment[0], &result_densities[0], frame_result_mask.data);

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (frame_result_mask.data[i * width + j] < 128) {
                        uint8_t *base_ptr = &frame_rgba->data[i * (width * 4) + j * 4];
                        base_ptr[0] = 0;
                        base_ptr[1] = 255 - frame_result_mask.data[i * width + j];
                        base_ptr[2] = 0;
                    }
                }
            }
            return std::unique_ptr<Frame>(new RGBAFrame(frame_rgba));
        }
        return frame;
    }
};
