#include "image-cvt.hpp"

namespace tsumaki {
    template <typename T>
    static inline T clamp(T val, T lo, T hi) {
        if (val < lo) return lo;
        if (val > hi) return hi;
        return val;
    }

    static inline void yuv2rgb(const float *color_matrix, uint8_t yuv[3], uint8_t *result) {
        uint8_t y = yuv[0], u = yuv[1], v = yuv[2];
        // color_matrix is column major
        float r = color_matrix[0] * y + color_matrix[1] * u + color_matrix[2]  * v + color_matrix[3]  * 1;
        float g = color_matrix[4] * y + color_matrix[5] * u + color_matrix[6]  * v + color_matrix[7]  * 1;
        float b = color_matrix[8] * y + color_matrix[9] * u + color_matrix[10] * v + color_matrix[11] * 1;

        r = clamp<float>(r, 0, 255);
        g = clamp<float>(g, 0, 255);
        b = clamp<float>(b, 0, 255);
        result[0] = (uint8_t)r;
        result[1] = (uint8_t)g;
        result[2] = (uint8_t)b;
    }

    ConvertedRGBAImage ConvertedRGBAImage::make_black(int width, int height) {
        ConvertedRGBAImage result(width, height);
        std::fill_n(result.data, result.get_size(), 0);
        return result;
    }

    ConvertedRGBAImage ConvertedRGBAImage::make_white(int width, int height) {
        ConvertedRGBAImage result(width, height);
        std::fill_n(result.data, result.get_size(), 255);
        return result;
    }

    ConvertedRGBAImage BaseRGBXFormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);
        int ri = get_red_position(), gi = get_blue_position(), bi = get_red_position(), ai = get_alpha_position();

        const uint8_t *plane = convertable->get_plane(0);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                const uint8_t *pixel_ptr = &plane[width*4*i + 4*j];
                uint8_t r = pixel_ptr[ri];
                uint8_t g = pixel_ptr[gi];
                uint8_t b = pixel_ptr[bi];
                uint8_t a;
                if (ai == -1) {
                    a = 255;
                } else {
                    a = pixel_ptr[ai];
                }
                result.set_pixel(i, j, r, g, b, a);
            }
        }
        return result;
    }

    ConvertedRGBAImage I420FormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);
        int y_line_size = convertable->get_line_size(0);
        int u_line_size = convertable->get_line_size(1);
        int v_line_size = convertable->get_line_size(2);

        const float *color_matrix = convertable->get_color_matrix();
        const uint8_t* planes[3] { convertable->get_plane(0), convertable->get_plane(1), convertable->get_plane(2) };
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][y_line_size * i + j],
                    planes[1][u_line_size * i / 2 + j / 2],
                    planes[2][v_line_size * i / 2 + j / 2]
                };
                uint8_t rgb[3];
                yuv2rgb(color_matrix, yuv, rgb);
                result.set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }
        return result;
    }

    ConvertedRGBAImage NV12FormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);

        int y_line_size = convertable->get_line_size(0);
        int chroma_line_size = convertable->get_line_size(1);

        const uint8_t* planes[2] { convertable->get_plane(0), convertable->get_plane(1) };
        const float *color_matrix = convertable->get_color_matrix();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][y_line_size * i + j],
                    planes[1][chroma_line_size * i + j / 2],
                    planes[1][chroma_line_size * i + j / 2 + 1]
                };
                uint8_t rgb[3];
                yuv2rgb(color_matrix, yuv, rgb);
                result.set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }

        return result;
    }

    ConvertedRGBAImage Packed422YUVFormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);
        int y1i = get_first_y_position();
        int y2i = get_second_y_position();
        int ui = get_u_position();
        int vi = get_v_position();

        const uint8_t *plane = convertable->get_plane(0);
        const float *color_matrix = convertable->get_color_matrix();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width * 2; j += 4) {
                const uint8_t *pixel_ptr = &plane[width*4*i + 4*j];
                uint8_t y1 = pixel_ptr[y1i];
                uint8_t y2 = pixel_ptr[y2i];
                uint8_t u = pixel_ptr[ui];
                uint8_t v = pixel_ptr[vi];

                uint8_t yuv_1[3] { y1, u, v };
                uint8_t yuv_2[3] { y2, u, v };

                uint8_t rgb1[3], rgb2[3];
                yuv2rgb(color_matrix, yuv_1, rgb1);
                yuv2rgb(color_matrix, yuv_2, rgb2);


                int data_j = j / 2;
                result.set_pixel(i, data_j, rgb1[0], rgb1[1], rgb1[2], 255);
                result.set_pixel(i, data_j + 1, rgb2[0], rgb2[1], rgb2[2], 255);
            }
        }
        return result;
    }

    ConvertedRGBAImage Y800FormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);

        const uint8_t *plane = convertable->get_plane(0);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t pixel_val = plane[width*i + j];
                result.data[width*4*i + 4*j + 0] = pixel_val;
                result.data[width*4*i + 4*j + 1] = pixel_val;
                result.data[width*4*i + 4*j + 2] = pixel_val;
                result.data[width*4*i + 4*j + 3] = pixel_val;
            }
        }
        return result;
    }

    ConvertedRGBAImage I444FormatCvt::convert() {
        int width = convertable->get_width();
        int height = convertable->get_height();
        ConvertedRGBAImage result(width, height);
        const float *color_matrix = convertable->get_color_matrix();
        const uint8_t* planes[3] { convertable->get_plane(0), convertable->get_plane(1), convertable->get_plane(2) };
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][width*i + j],
                    planes[1][width*i + j],
                    planes[2][width*i + j]
                };
                uint8_t rgb[3];
                yuv2rgb(color_matrix, yuv, rgb);
                result.set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }
        return result;
    }
}
