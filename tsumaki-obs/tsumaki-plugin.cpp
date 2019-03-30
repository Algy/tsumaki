#include <obs-module.h>
#include <chrono>
#include "tsumaki-filter.hpp"

OBS_DECLARE_MODULE()
/* Implements common ini-based locale (optional) */
OBS_MODULE_USE_DEFAULT_LOCALE("my-plugin", "en-US")

using tsumaki::TsumakiFilter;
using tsumaki::Frame;
using tsumaki::OBSFrame;
typedef std::chrono::high_resolution_clock Clock;


static struct obs_source_frame* do_filter(TsumakiFilter* filter, struct obs_source_frame *obs_frame) {
    unique_ptr<Frame> input_frame(filter->wrap_obs_frame(obs_frame));
    unique_ptr<Frame> output_frame = filter->frame_update(std::move(input_frame));

    // Case 1. If filter_update returned nullptr, return nullptr and skip the current frame. 
    if (output_frame == nullptr) {
        return static_cast<struct obs_source_frame *>(nullptr);
    }

    // Case 2. If filter_update returned an OBSFrame, we don't need to convert its format.
    // By this optimization, useless copy operations can be prevented.
    {
        OBSFrame *would_be_obs_frame = dynamic_cast<OBSFrame *>(output_frame.get());
        if (would_be_obs_frame != nullptr) {
            would_be_obs_frame->transfer_ownership();
            return would_be_obs_frame->get_obs_frame();
        }
    }

    // Case 3. If filter_update returned else, build a rgba buffer and convert it back to the same content of the original format.
    OBSFrame converted_frame(*output_frame, obs_frame);
    converted_frame.transfer_ownership();
    return converted_frame.get_obs_frame();
}

struct obs_source_info tsumaki_filter = {
	.id = "tsumaki_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_ASYNC,
	.get_name = [] (void *unused) { UNUSED_PARAMETER(unused); return obs_module_text("Tsumaki"); },
	.create = [] (obs_data_t *settings, obs_source_t *context) {
        TsumakiFilter *filter = new TsumakiFilter();
        filter->set_context(context);
        filter->update_settings(settings);
        blog(LOG_INFO, "[Tsumaki] tsumaki has been created");
        return static_cast<void *>(filter);
    },
	.destroy = [] (void *data) { delete static_cast<TsumakiFilter *>(data); },
	.get_properties = [] (void *data) {
        obs_properties_t *props = obs_properties_create();
        static_cast<TsumakiFilter *>(data)->get_properties(props);
        blog(LOG_INFO, "[Tsumaki] Properties initialized");
        return props;
    },
	.update = [] (void *data, obs_data_t *settings) {
        static_cast<TsumakiFilter *>(data)->update_settings(settings);
        blog(LOG_INFO, "[Tsumaki] Updated settings");
    },
	.filter_video = [] (void *data, struct obs_source_frame *obs_frame) {
        TsumakiFilter* filter = static_cast<TsumakiFilter *>(data);

        // auto fn = [] { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); };
        struct obs_source_frame* filtered_obs_frame = do_filter(filter, obs_frame);
        return filtered_obs_frame;
    },
	.filter_remove = [] (void *data, obs_source_t *parent) {
        static_cast<TsumakiFilter *>(data)->detach(parent);
    }
};


extern "C" {
    bool obs_module_load(void) {
        obs_register_source(&tsumaki_filter);
        return true;
    }
}
