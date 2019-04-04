#include "obs-filter.hpp"

namespace tsumaki {
    static unique_ptr<VideoFormatCvt>
    make_video_format_cvt(const BaseVideoConvertable &convertable, struct obs_source_frame *frame) {
        VideoFormatCvt *result_ptr = nullptr;
        switch (frame->format) {
        case VIDEO_FORMAT_NONE:
            result_ptr = new NoneFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_I420:
            result_ptr = new I420FormatCvt(convertable);
            break;
        case VIDEO_FORMAT_NV12:
            result_ptr = new NV12FormatCvt(convertable);
            break;
        case VIDEO_FORMAT_YVYU:
            result_ptr = new YVYUFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_YUY2:
            result_ptr = new YUY2FormatCvt(convertable);
            break;
        case VIDEO_FORMAT_UYVY:
            result_ptr = new UYVYFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_RGBA:
            result_ptr = new RGBAFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_BGRA:
            result_ptr = new BGRAFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_BGRX:
            result_ptr = new BGRXFormatCvt(convertable);
            break;
        case VIDEO_FORMAT_Y800:
            result_ptr = new Y800FormatCvt(convertable);
            break;
        case VIDEO_FORMAT_I444:
            result_ptr = new I444FormatCvt(convertable);
            break;
        default:
            result_ptr = new NoneFormatCvt(convertable);
            break;
        }
        std::unique_ptr<VideoFormatCvt> result(result_ptr);
        return result;
    }
    std::unique_ptr<OBSFrame> OBSFilter::wrap_obs_frame(struct obs_source_frame* frame) {
        return std::unique_ptr<OBSFrame>(new OBSFrame(context, frame));
    }

    const char* OBSFilter::T(const char* lookup_str) {
        return obs_module_text(lookup_str);
    }

    OBSFrame::OBSFrame(const OBSFrame &other) {
        context = other.context;
        frame = obs_source_frame_create(
            other.frame->format,
            other.frame->width,
            other.frame->height
        );
        frame->timestamp = other.frame->timestamp;
        obs_source_frame_copy(frame, other.frame);
        frame->refs = 1;
        image_cache = other.image_cache;
    }

    OBSFrame::OBSFrame(const Frame &other, struct obs_source_frame* reference_frame) {
        frame = obs_source_frame_create(reference_frame->format, other.get_width(), other.get_height());
        frame->timestamp = reference_frame->timestamp;
        std::copy(reference_frame->color_matrix, reference_frame->color_matrix + 16, frame->color_matrix);
        frame->full_range = reference_frame->full_range;
        std::copy(reference_frame->color_range_min, reference_frame->color_range_min + 3, frame->color_range_min);
        std::copy(reference_frame->color_range_max, reference_frame->color_range_max + 3, frame->color_range_max);
        frame->flip = reference_frame->flip;
        frame->refs = 1;

        // Copy frame
        auto cvt = make_video_format_cvt(*this, reference_frame); 
        cvt->inverse_convert(*other.get_rgba_image());
    }

    shared_ptr<ConvertedRGBAImage> OBSFrame::get_rgba_image() const {
        if (image_cache != nullptr) {
            return image_cache;
        }
        auto cvt = make_video_format_cvt(*this, frame);
        image_cache = cvt->convert();
        return image_cache;
    }

    int OBSFrame::get_num_planes() const {
        unique_ptr<VideoFormatCvt> cvt = make_video_format_cvt(*this, frame);
        return cvt->get_num_planes();
    }
}
