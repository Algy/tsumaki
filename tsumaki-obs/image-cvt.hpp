#include <memory>
#include <vector>
#include <algorithm>

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
            std::copy(data, other.data, other.data + get_size());
        }

        ConvertedRGBAImage& operator=(const ConvertedRGBAImage& other) {
            if (&other != this) {
                return *this;
            }
            delete [] data;
            width = other.width;
            height = other.height;
            data = new uint8_t[get_size()];
            std::copy(data, other.data, other.data + get_size());
            return *this;
        }

        ~ConvertedRGBAImage() {
            delete [] data;
        }

        int get_size() { return width * height * 4; };
        void set_pixel(int i, int j, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            data[width*4*i + 4*j + 0] = r;
            data[width*4*i + 4*j + 1] = g;
            data[width*4*i + 4*j + 2] = b;
            data[width*4*i + 4*j + 3] = a;
        };

        static ConvertedRGBAImage make_black(int width, int height);
        static ConvertedRGBAImage make_white(int width, int height);
    };

    class BaseVideoConvertable {
    public:
        virtual int get_width() = 0;
        virtual int get_height() = 0;
        virtual const float* get_color_matrix() = 0;
        virtual int get_line_size(int plane_index) = 0;
        virtual const uint8_t* get_plane(int plane_index) = 0;
        virtual ~BaseVideoConvertable() {};
    };

    class VideoFormatCvt {
    protected:
        std::shared_ptr<BaseVideoConvertable> convertable;
    public:
        VideoFormatCvt(std::shared_ptr<BaseVideoConvertable> convertable) : convertable(convertable) {};
        virtual ~VideoFormatCvt() {};
    public:
        virtual ConvertedRGBAImage convert() = 0;
    };

    class NoneFormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() { return 0; }
        ConvertedRGBAImage convert() { return ConvertedRGBAImage::make_black(convertable->get_width(), convertable->get_height()); }
    };

    class I420FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() { return 3; }
        ConvertedRGBAImage convert();
    };

    class NV12FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() { return 3; }
        ConvertedRGBAImage convert();
    };

    class Packed422YUVFormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() { return 1; }
        virtual int get_first_y_position() = 0;
        virtual int get_second_y_position() = 0;
        virtual int get_u_position() = 0;
        virtual int get_v_position() = 0;
        ConvertedRGBAImage convert();
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
        int get_num_planes() { return 1; };
        virtual int get_red_position() = 0;
        virtual int get_green_position() = 0;
        virtual int get_blue_position() = 0;
        virtual int get_alpha_position() = 0;
        ConvertedRGBAImage convert();
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
        int get_num_planes() { return 1; };
        ConvertedRGBAImage convert();
    };

    class I444FormatCvt : public VideoFormatCvt {
    public:
        using VideoFormatCvt::VideoFormatCvt;
        int get_num_planes() { return 3; };
        ConvertedRGBAImage convert();
    };

}
