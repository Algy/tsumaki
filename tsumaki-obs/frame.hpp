#pragma once
#include <memory>
#include "image-cvt.hpp"

namespace tsumaki {
    class Frame : public BaseVideoConvertable {
    public:
        virtual std::shared_ptr<ConvertedRGBAImage> get_rgba_image() const = 0;
    };

    class RGBAFrame : public Frame {
    private:
        std::shared_ptr<ConvertedRGBAImage> image;
    public:
        RGBAFrame(std::shared_ptr<ConvertedRGBAImage> image) : image(image) {};
        int get_width() const { return image->width; };
        int get_height() const { return image->height; };
        const float* get_color_matrix() const;
        int get_line_size(int plane_index) const { (void)plane_index; return image->width * 4; };
        const uint8_t* get_plane(int plane_index) const { (void)plane_index; return image->data; };
        uint8_t* get_modifiable_plane(int plane_index) const {
            (void)plane_index;
            return image->data;
        };
        int get_num_planes() const { return 1; };
    public:
        std::shared_ptr<ConvertedRGBAImage> get_rgba_image() const { return image; };
    };
}
