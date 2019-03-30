#include "image-cvt.hpp"
#include "tsumaki-math.hpp"

namespace tsumaki {
    template <typename T>
    static inline T clamp(T val, T lo, T hi) {
        if (val < lo) return lo;
        if (val > hi) return hi;
        return val;
    }

    static inline void cvtcolor(const float *color_matrix, uint8_t *yuv, uint8_t *result) {
        float y = (float)yuv[0], u = (float)yuv[1], v = (float)yuv[2];
        // color_matrix is column major
        float r = color_matrix[0] * y + color_matrix[1] * u + color_matrix[2]  * v + color_matrix[3]  * 255;
        float g = color_matrix[4] * y + color_matrix[5] * u + color_matrix[6]  * v + color_matrix[7]  * 255;
        float b = color_matrix[8] * y + color_matrix[9] * u + color_matrix[10] * v + color_matrix[11] * 255;

        r = clamp<float>(r, 0, 255);
        g = clamp<float>(g, 0, 255);
        b = clamp<float>(b, 0, 255);
        result[0] = r;
        result[1] = g;
        result[2] = b;
    }

    shared_ptr<ConvertedRGBAImage> ConvertedRGBAImage::make_black(int width, int height) {
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        std::fill_n(result->data, result->get_size(), 0);
        return result;
    }

    shared_ptr<ConvertedRGBAImage> ConvertedRGBAImage::make_white(int width, int height) {
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        std::fill_n(result->data, result->get_size(), 255);
        return result;
    }

    shared_ptr<ConvertedRGBAImage> BaseRGBXFormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int ri = get_red_position(), gi = get_blue_position(), bi = get_red_position(), ai = get_alpha_position();

        const uint8_t *plane = convertable.get_plane(0);
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
                result->set_pixel(i, j, r, g, b, a);
            }
        }
        return result;
    }

    void BaseRGBXFormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int ri = get_red_position(), gi = get_blue_position(), bi = get_red_position(), ai = get_alpha_position();

        uint8_t *plane = convertable.get_modifiable_plane(0);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t rgba[4];
                image.get_pixel(i, j, rgba);
                uint8_t *pixel_ptr = &plane[width*4*i + 4*j];

                pixel_ptr[ri] = rgba[0];
                pixel_ptr[gi] = rgba[1];
                pixel_ptr[bi] = rgba[2];
                if (ai != -1) {
                    pixel_ptr[ai] = rgba[3];
                }
            }
        }
    }

    void BaseVideoConvertable::get_inverse_color_matrix(float *inv_4by4) const {
        inverse_4by4(get_color_matrix(), inv_4by4);
    }

    shared_ptr<ConvertedRGBAImage> I420FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int y_line_size = convertable.get_line_size(0);
        int u_line_size = convertable.get_line_size(1);
        int v_line_size = convertable.get_line_size(2);

        const float *color_matrix = convertable.get_color_matrix();
        const uint8_t* planes[3] { convertable.get_plane(0), convertable.get_plane(1), convertable.get_plane(2) };
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][y_line_size * i + j],
                    planes[1][u_line_size * (i / 2) + j / 2],
                    planes[2][v_line_size * (i / 2) + j / 2]
                };
                uint8_t rgb[3];
                cvtcolor(color_matrix, yuv, rgb);
                result->set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }
        return result;
    }

    void I420FormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);

        int y_line_size = convertable.get_line_size(0);
        int u_line_size = convertable.get_line_size(1);
        int v_line_size = convertable.get_line_size(2);

        uint8_t *plane_0 = convertable.get_modifiable_plane(0);
        uint8_t *plane_1 = convertable.get_modifiable_plane(1);
        uint8_t *plane_2 = convertable.get_modifiable_plane(2);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t rgba[4], yuv[3];
                image.get_pixel(i, j, rgba);
                cvtcolor(inv_color_matrix, rgba, yuv);
                plane_0[y_line_size * i + j] = yuv[0];
                plane_1[u_line_size * (i / 2) + j / 2] = yuv[1];
                plane_2[v_line_size * (i / 2) + j / 2] = yuv[2];
            }
        }
    }

    shared_ptr<ConvertedRGBAImage> NV12FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));

        int y_line_size = convertable.get_line_size(0);
        int chroma_line_size = convertable.get_line_size(1);

        const uint8_t* planes[2] { convertable.get_plane(0), convertable.get_plane(1) };
        const float *color_matrix = convertable.get_color_matrix();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][y_line_size * i + j],
                    planes[1][chroma_line_size * i + j / 2],
                    planes[1][chroma_line_size * i + j / 2 + 1]
                };
                uint8_t rgb[3];
                cvtcolor(color_matrix, yuv, rgb);
                result->set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }

        return result;
    }

    void NV12FormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);

        int y_line_size = convertable.get_line_size(0);
        int chroma_line_size = convertable.get_line_size(1);

        uint8_t *plane_0 = convertable.get_modifiable_plane(0);
        uint8_t *plane_1 = convertable.get_modifiable_plane(1);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t rgba[4], yuv[3];
                image.get_pixel(i, j, rgba);
                cvtcolor(inv_color_matrix, rgba, yuv);
                plane_0[y_line_size * i + j] = yuv[0];
                plane_1[chroma_line_size * i + j / 2] = yuv[1];
                plane_1[chroma_line_size * i + j / 2 + 1] = yuv[2];
            }
        }
    }

    shared_ptr<ConvertedRGBAImage> Packed422YUVFormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int y1i = get_first_y_position();
        int y2i = get_second_y_position();
        int ui = get_u_position();
        int vi = get_v_position();

        const uint8_t *plane = convertable.get_plane(0);
        const float *color_matrix = convertable.get_color_matrix();
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
                cvtcolor(color_matrix, yuv_1, rgb1);
                cvtcolor(color_matrix, yuv_2, rgb2);


                int data_j = j / 2;
                result->set_pixel(i, data_j, rgb1[0], rgb1[1], rgb1[2], 255);
                result->set_pixel(i, data_j + 1, rgb2[0], rgb2[1], rgb2[2], 255);
            }
        }
        return result;
    }

    void Packed422YUVFormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);

        int y1i = get_first_y_position();
        int y2i = get_second_y_position();
        int ui = get_u_position();
        int vi = get_v_position();

        uint8_t* plane = convertable.get_modifiable_plane(0);
        for (int i = 0; i < height; i++) {
            int eff_width = (width / 2) * 2;
            for (int j = 0; j < eff_width; j += 2) {
                uint8_t rgba1[4], rgba2[4];

                image.get_pixel(i, j, rgba1);
                image.get_pixel(i, j+1, rgba2);

                uint8_t yuv_1[3], yuv_2[3];
                cvtcolor(inv_color_matrix, rgba1, yuv_1);
                cvtcolor(inv_color_matrix, rgba2, yuv_2);

                uint8_t *pixel_ptr = &plane[width*4*i + 4*j];
                pixel_ptr[y1i] = yuv_1[0];
                pixel_ptr[y2i] = yuv_2[0];
                pixel_ptr[ui] = yuv_2[1];
                pixel_ptr[vi] = yuv_2[2];
            }
        }
    }


    shared_ptr<ConvertedRGBAImage> Y800FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));

        const uint8_t *plane = convertable.get_plane(0);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t pixel_val = plane[width*i + j];
                uint8_t* data = &result->data[width*4*i + 4*j];
                data[0] = data[1] = data[2] = data[3] = pixel_val;
            }
        }
        return result;
    }

    void Y800FormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();
        uint8_t *plane = convertable.get_modifiable_plane(0);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t rgba[4];
                image.get_pixel(i, j, rgba);
                plane[width*i + j] = (uint8_t)(((uint32_t)rgba[0] + (uint32_t)rgba[1] + (uint32_t)rgba[2]) / 3);
            }
        }
    }

    shared_ptr<ConvertedRGBAImage> I444FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        const float *color_matrix = convertable.get_color_matrix();
        const uint8_t* planes[3] { convertable.get_plane(0), convertable.get_plane(1), convertable.get_plane(2) };
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t yuv[3] {
                    planes[0][width*i + j],
                    planes[1][width*i + j],
                    planes[2][width*i + j]
                };
                uint8_t rgb[3];
                cvtcolor(color_matrix, yuv, rgb);
                result->set_pixel(i, j, rgb[0], rgb[1], rgb[2], 255);
            }
        }
        return result;
    }

    void I444FormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);

        uint8_t *plane_0 = convertable.get_modifiable_plane(0);
        uint8_t *plane_1 = convertable.get_modifiable_plane(1);
        uint8_t *plane_2 = convertable.get_modifiable_plane(1);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                uint8_t rgba[4], yuv[3];
                image.get_pixel(i, j, rgba);
                cvtcolor(inv_color_matrix, rgba, yuv);
                plane_0[width*i + j] = yuv[0];
                plane_1[width*i + j] = yuv[1];
                plane_2[width*i + j] = yuv[2];
            }
        }
    }
}
