#pragma once
#include <obs-module.h>
#include <memory>
#include <algorithm>
#include "image-cvt.hpp"

using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;

namespace tsumaki {
    class Frame : public BaseVideoConvertable {
    public:
        virtual shared_ptr<ConvertedRGBAImage> get_rgba_image() const = 0;
    };

    class OBSFrame : public Frame {
    private:
        struct obs_source_frame *frame;
        obs_source_t* context;
        bool ownership = true;
        mutable shared_ptr<ConvertedRGBAImage> image_cache;
    public:
        struct obs_source_frame *get_obs_frame() { return frame; };

        void transfer_ownership() { ownership = false; };
    public:
        OBSFrame(obs_source_t* context, struct obs_source_frame *frame) : frame(frame), context(context) {};
        OBSFrame(const OBSFrame &other);
        OBSFrame(const Frame &other, struct obs_source_frame* reference_frame);

        OBSFrame& operator=(const OBSFrame &other) = delete; // not copyable
        virtual ~OBSFrame() {
            if (ownership) {
                obs_source_release_frame(obs_filter_get_parent(context), frame);
            }
        };

    public:
        int get_width() const { return frame->width; };
        int get_height() const { return frame->height; };
        const float* get_color_matrix() const { return frame->color_matrix; };
        int get_line_size(int plane_index) const { (void)plane_index ; return frame->linesize[plane_index]; };
        int get_num_planes() const;
        const uint8_t* get_plane(int plane_index) const { (void)plane_index; return frame->data[plane_index]; };

        uint8_t* get_modifiable_plane(int plane_index) const { return frame->data[plane_index]; };
    public:
        shared_ptr<ConvertedRGBAImage> get_rgba_image() const;
    };

    const float identity_color_matrix[16] {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    class RGBAFrame : public Frame {
    private:
        shared_ptr<ConvertedRGBAImage> image;
    public:
        RGBAFrame(shared_ptr<ConvertedRGBAImage> image) : image(image) {};
        int get_width() const { return image->width; };
        int get_height() const { return image->height; };
        const float* get_color_matrix() const { return identity_color_matrix; };
        int get_line_size(int plane_index) const { (void)plane_index; return image->width * 4; };
        const uint8_t* get_plane(int plane_index) const { (void)plane_index; return image->data; };
        uint8_t* get_modifiable_plane(int plane_index) const {
            (void)plane_index;
            return image->data;
        };
        int get_num_planes() const { return 1; };
    public:
        shared_ptr<ConvertedRGBAImage> get_rgba_image() const { return image; };
    };

    class OBSFilter {
    private:
        obs_source_t* context = nullptr;

    public:
        virtual ~OBSFilter() {};
        void set_context(obs_source_t* context) { this->context = context; };
        obs_source_t* get_context() { return context; };
    public:
        virtual void update_settings(obs_data_t *settings) = 0;
        virtual void get_properties(obs_properties_t* props) = 0;
        virtual void detach(obs_source_t *parent) = 0;
        virtual unique_ptr<Frame> frame_update(unique_ptr<Frame> frame) = 0;
    public:
        unique_ptr<OBSFrame> wrap_obs_frame(struct obs_source_frame* frame);

    protected:
        const char* T(const char* lookup_str) { return obs_module_text(lookup_str); }; // translate
    };

    class TsumakiFilter : public OBSFilter {
    private:
        uint64_t interval;
    public:
        TsumakiFilter();
        ~TsumakiFilter();
        void update_settings(obs_data_t *settings);
        void get_properties(obs_properties_t* props);
        void detach(obs_source_t *parent);
        unique_ptr<Frame> frame_update(unique_ptr<Frame> frame);
    };
};

