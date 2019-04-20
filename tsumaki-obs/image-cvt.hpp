#pragma once
#include <memory>
#include <vector>
#include <algorithm>

using std::unique_ptr;
using std::shared_ptr;
using std::vector;
namespace tsumaki {
    class ConvertedRGBAImage {
    public:
        uint8_t* data;
        int width;
        int height;
    public:
        ConvertedRGBAImage(int width, int height): width(width), height(height) {
            data = new uint8_t[get_size()];
        }

        ConvertedRGBAImage(const ConvertedRGBAImage& other): width(other.width), height(other.height) {
            data = new uint8_t[get_size()];
            std::copy(other.data, other.data + get_size(), data);
        }

        ConvertedRGBAImage& operator=(const ConvertedRGBAImage& other) {
            if (this == &other) {
                return *this;
            }
            delete [] data;
            width = other.width;
            height = other.height;
            data = new uint8_t[get_size()];
            std::copy(other.data, other.data + get_size(), data);
            return *this;
        }

        ~ConvertedRGBAImage() {
            delete [] data;
        }

        int get_size() const { return width * height * 4; };
        void get_pixel(int i, int j, uint8_t *rgba) const {
            rgba[0] = data[width*4*i + 4*j + 0];
            rgba[1] = data[width*4*i + 4*j + 1];
            rgba[2] = data[width*4*i + 4*j + 2];
            rgba[3] = data[width*4*i + 4*j + 3];
        }

        void set_pixel(int i, int j, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            int index = width*4*i + 4*j;
            data[index] = r;
            data[index + 1] = g;
            data[index + 2] = b;
            data[index + 3] = a;
        };

        static shared_ptr<ConvertedRGBAImage> make_black(int width, int height);
        static shared_ptr<ConvertedRGBAImage> make_white(int width, int height);

        ConvertedRGBAImage resize_bilinear(int new_width, int new_height);
    };

    class BaseVideoConvertable {
    public:
        virtual int get_width() const = 0;
        virtual int get_height() const = 0;
        virtual const float* get_color_matrix() const = 0;
        virtual int get_line_size(int plane_index) const = 0;
        virtual int get_num_planes() const = 0;
        virtual const uint8_t* get_plane(int plane_index) const = 0;
        virtual uint8_t* get_modifiable_plane(int plane_index) const = 0;
        virtual ~BaseVideoConvertable() {};
        void get_inverse_color_matrix(float *inv_4by4) const;
    };

    class VideoFormatCvt {
    protected:
        const BaseVideoConvertable& convertable; // borrowed
    public:
        VideoFormatCvt(const BaseVideoConvertable& convertable) : convertable(convertable) {};

        virtual int get_num_planes() const { return 0; };
        virtual ~VideoFormatCvt() {};
    public:
        virtual shared_ptr<ConvertedRGBAImage> convert() = 0;
        virtual void inverse_convert(const ConvertedRGBAImage &image) = 0;
    };

    class NoneFormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 0; }
        shared_ptr<ConvertedRGBAImage> convert() { return ConvertedRGBAImage::make_black(convertable.get_width(), convertable.get_height()); }
        void inverse_convert(const ConvertedRGBAImage &image) { (void)image; };
    };

    class I420FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 3; }
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

    class NV12FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 2; }
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

    class Packed422YUVFormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 1; }
        virtual int get_first_y_position() = 0;
        virtual int get_second_y_position() = 0;
        virtual int get_u_position() = 0;
        virtual int get_v_position() = 0;
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

    class YVYUFormatCvt : public Packed422YUVFormatCvt {
    public:
        using Packed422YUVFormatCvt::Packed422YUVFormatCvt;
        int get_first_y_position() { return 0; };
        int get_second_y_position() { return 2; };
        int get_u_position() { return 3; };
        int get_v_position() { return 1; };
    };

    class YUY2FormatCvt : public Packed422YUVFormatCvt {
    public:
        using Packed422YUVFormatCvt::Packed422YUVFormatCvt;
        int get_first_y_position() { return 0; };
        int get_second_y_position() { return 2; };
        int get_u_position() { return 1; };
        int get_v_position() { return 3; };
    };

    class UYVYFormatCvt : public Packed422YUVFormatCvt {
    public:
        using Packed422YUVFormatCvt::Packed422YUVFormatCvt;
        int get_first_y_position() { return 1; };
        int get_second_y_position() { return 3; };
        int get_u_position() { return 0; };
        int get_v_position() { return 2; };
    };

    class BaseRGBXFormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 1; };
        virtual int get_red_position() = 0;
        virtual int get_green_position() = 0;
        virtual int get_blue_position() = 0;
        virtual int get_alpha_position() = 0;
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

    class RGBAFormatCvt : public BaseRGBXFormatCvt {
    public:
        using BaseRGBXFormatCvt::BaseRGBXFormatCvt;
        int get_red_position() { return 0; };
        int get_green_position() { return 1; };
        int get_blue_position() { return 2; };
        int get_alpha_position() { return 3; };
    };

    class BGRAFormatCvt : public BaseRGBXFormatCvt {
    public:
        using BaseRGBXFormatCvt::BaseRGBXFormatCvt;
        int get_red_position() { return 2; };
        int get_green_position() { return 1; };
        int get_blue_position() { return 0; };
        int get_alpha_position() { return 3; };
    };

    class BGRXFormatCvt : public BaseRGBXFormatCvt {
    public:
        using BaseRGBXFormatCvt::BaseRGBXFormatCvt;
        int get_red_position() { return 2; };
        int get_green_position() { return 1; };
        int get_blue_position() { return 0; };
        int get_alpha_position() { return -1; };
    };

    class Y800FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 1; };
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

    class I444FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() const { return 3; };
        shared_ptr<ConvertedRGBAImage> convert();
        void inverse_convert(const ConvertedRGBAImage &image);
    };

}
