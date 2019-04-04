#pragma once
#include <string>
#include <obs-module.h>
#include <memory>
#include <mutex>
#include "frame.hpp"

namespace tsumaki {
    class OBSFrame;
    class OBSLoggable;
    class OBSLoggerEndl {};
    class OBSLogger {
    private:
        int level;
        const OBSLoggable &filter; // weak reference
        const std::string scope_name;
        std::string line_buffer;
        std::mutex mutex;
    public:
        OBSLogger(int level, const OBSLoggable &filter) : level(level), filter(filter) {};
    public:
        OBSLoggerEndl endl;
    public:
        OBSLogger& operator<<(int);
        OBSLogger& operator<<(float);
        OBSLogger& operator<<(double);
        OBSLogger& operator<<(const char *);
        OBSLogger& operator<<(const std::string&);
        OBSLogger& operator<<(const OBSLoggerEndl&);
    };

    class OBSLoggable {
    public:
        OBSLogger error;
        OBSLogger warn;
        OBSLogger info;
        OBSLogger debug;
    public:
        OBSLoggable() : error(LOG_ERROR, *this), warn(LOG_WARNING, *this), info(LOG_INFO, *this), debug(LOG_DEBUG, *this) {};
    public:
        virtual const char* get_scope_name() const = 0;
    };


    class OBSFilter : public OBSLoggable {
    private:
        obs_source_t* context;
    public:
        OBSFilter() : OBSLoggable()  {};
        virtual ~OBSFilter() {};
        void set_context(obs_source_t* context) { this->context = context; };
        obs_source_t* get_context() { return context; };
    public:
        virtual void run_once() {};
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update_settings(obs_data_t *settings) = 0;
        virtual void get_properties(obs_properties_t* props) = 0;
        virtual void detach(obs_source_t *parent) = 0;
        virtual std::unique_ptr<Frame> frame_update(std::unique_ptr<Frame> frame) = 0;
    public:
        std::unique_ptr<OBSFrame> wrap_obs_frame(struct obs_source_frame* frame);
    protected:
        const char* T(const char* lookup_str); // translate
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

}


