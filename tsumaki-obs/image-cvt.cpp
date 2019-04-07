#include "image-cvt.hpp"
#include "tsumaki-math.hpp"

#define FAST_CVT_601(yuv, rgb) { \
    int dy = (int)yuv[0] - 16, du = (int)yuv[1] - 128, dv = (int)yuv[2] - 128; \
    int r = (298 * dy + 409 * dv + 128) >> 8; \
    int g = (298 * dy + (-100) * du + (-208) * dv + 128) >> 8; \
    int b = (298 * dy + 516 * du + 128) >> 8; \
    rgb[0] = clamp<int>(r, 0, 255); \
    rgb[1] = clamp<int>(g, 0, 255); \
    rgb[2] = clamp<int>(b, 0, 255); \
}

#define FAST_CVT_709(yuv, rgb) { \
    int dy = (int)yuv[0] - 16, du = (int)yuv[1] - 128, dv = (int)yuv[2] - 128; \
    int r = (298 * dy + 459 * dv + 128) >> 8; \
    int g = (298 * dy + (-55) * du + (-136) * dv + 128) >> 8; \
    int b = (298 * dy + 541 * du + 128) >> 8; \
    rgb[0] = clamp<int>(r, 0, 255); \
    rgb[1] = clamp<int>(g, 0, 255); \
    rgb[2] = clamp<int>(b, 0, 255); \
}


#define FAST_INV_CVT_601(rgb, yuv) { \
    int r = rgb[0], g = rgb[1], b = rgb[2]; \
    int y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16; \
    int u = ((-38 * r + -74 * g + 112 * b + 128) >> 8) + 128; \
    int v = ((112 * r + -94 * g + -18 * b + 128) >> 8) + 128; \
    yuv[0] = clamp<int>(y, 0, 255); \
    yuv[1] = clamp<int>(u, 0, 255); \
    yuv[2] = clamp<int>(v, 0, 255);\
}

#define FAST_INV_CVT_709(yuv, rgb) { \
    int r = rgb[0], g = rgb[1], b = rgb[2]; \
    int y = ((47 * r + 157 * g + 16 * b + 128) >> 8) + 16; \
    int u = ((-26 * r + -87 * g + 112 * b + 128) >> 8) + 128; \
    int v = ((112 * r + -102 * g + -10 * b + 128) >> 8) + 128; \
    yuv[0] = clamp<int>(y, 0, 255); \
    yuv[1] = clamp<int>(u, 0, 255); \
    yuv[2] = clamp<int>(v, 0, 255);\
}



namespace tsumaki {
    template <typename T>
    static inline T clamp(T val, T lo, T hi) {
        return (val < lo)? lo : ((val > hi)? hi : val);
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

    void BaseVideoConvertable::get_inverse_color_matrix(float *inv_4by4) const {
        inverse_4by4(get_color_matrix(), inv_4by4);
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

    shared_ptr<ConvertedRGBAImage> I420FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int y_line_size = convertable.get_line_size(0);
        int u_line_size = convertable.get_line_size(1);

        const uint8_t* planes[3] { convertable.get_plane(0), convertable.get_plane(1), convertable.get_plane(2) };
        const uint8_t *y_plane = planes[0];

        uint8_t *data = result->data;
        for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width / 2; j++) {
                int chroma_index = u_line_size * i + j;
                uint8_t subsampled_u = planes[1][chroma_index];
                uint8_t subsampled_v = planes[2][chroma_index];

                int y_base = y_line_size * 2 * i + 2 * j;
                uint8_t yuv[4][3] = {
                    { y_plane[y_base], subsampled_u, subsampled_v },
                    { y_plane[y_base + 1] , subsampled_u, subsampled_v },
                    { y_plane[y_base + y_line_size], subsampled_u, subsampled_v },
                    { y_plane[y_base + y_line_size + 1], subsampled_u, subsampled_v }
                };

                uint8_t rgb[4][3];

                FAST_CVT_601(yuv[0], rgb[0]);
                FAST_CVT_601(yuv[1], rgb[1]);
                FAST_CVT_601(yuv[2], rgb[2]);
                FAST_CVT_601(yuv[3], rgb[3]);

                int index = 4 * width * 2 * i + 4 * 2 * j;

                data[index] = rgb[0][0];
                data[index + 1] = rgb[0][1];
                data[index + 2] = rgb[0][2];

                data[index + 4] = rgb[1][0];
                data[index + 5] = rgb[1][1];
                data[index + 6] = rgb[1][2];

                data[index + 4 * width] = rgb[2][0];
                data[index + 4 * width + 1] = rgb[2][1];
                data[index + 4 * width + 2] = rgb[2][2];

                data[index + 4 * width + 4] = rgb[3][0];
                data[index + 4 * width + 5] = rgb[3][1];
                data[index + 4 * width + 6] = rgb[3][2];
            }
        }
        return result;
    }

    void I420FormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        int y_line_size = convertable.get_line_size(0);
        int u_line_size = convertable.get_line_size(1);

        uint8_t *plane_0 = convertable.get_modifiable_plane(0);
        uint8_t *plane_1 = convertable.get_modifiable_plane(1);
        uint8_t *plane_2 = convertable.get_modifiable_plane(2);

        const uint8_t *data = image.data;
        for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width / 2; j++) {
                int chroma_index = u_line_size * i + j;

                int index = 4 * width * 2 * i + 4 * 2 * j;
                uint8_t rgb[4][3] = {
                    { data[index], data[index + 1], data[index + 2] },
                    { data[index + 4], data[index + 5], data[index + 6] },
                    { data[index + 4 * width], data[index + 4 * width + 1], data[index + 4 * width + 2] },
                    { data[index + 4 * width + 4], data[index + 4 * width + 5], data[index + 4 * width + 6] }
                };
                uint8_t yuv[4][3];

                int y_base = y_line_size * 2 * i + 2 * j;

                FAST_INV_CVT_601(rgb[0], yuv[0]);
                FAST_INV_CVT_601(rgb[1], yuv[1]);
                FAST_INV_CVT_601(rgb[2], yuv[2]);
                FAST_INV_CVT_601(rgb[3], yuv[3]);

                plane_1[chroma_index] = yuv[0][1];
                plane_2[chroma_index] = yuv[0][2];

                plane_0[y_base] = yuv[0][0];
                plane_0[y_base + 1] = yuv[1][0];
                plane_0[y_base + y_line_size] = yuv[2][0];
                plane_0[y_base + y_line_size + 1] = yuv[3][0];
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

        int packed_line_size = convertable.get_line_size(0);
        const uint8_t *plane = convertable.get_plane(0);
        uint8_t *data = result->data;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width * 2; j += 4) {
                const uint8_t *pixel_ptr = &plane[packed_line_size*i + j];
                uint8_t y1 = pixel_ptr[y1i];
                uint8_t y2 = pixel_ptr[y2i];
                uint8_t u = pixel_ptr[ui];
                uint8_t v = pixel_ptr[vi];

                uint8_t yuv_1[3] { y1, u, v };
                uint8_t yuv_2[3] { y2, u, v };

                uint8_t rgb1[3], rgb2[3];
                FAST_CVT_601(yuv_1, rgb1);
                FAST_CVT_601(yuv_2, rgb2);

                int index = width * 4 * i + 4 * (j / 2);
                data[index] = rgb1[0];
                data[index + 1] = rgb1[1];
                data[index + 2] = rgb1[2];
                data[index + 3] = 255;
                data[index + 4] = rgb2[0];
                data[index + 5] = rgb2[1];
                data[index + 6] = rgb2[2];
                data[index + 7] = 255;
            }
        }
        return result;
    }

    void Packed422YUVFormatCvt::inverse_convert(const ConvertedRGBAImage& image) {
        int width = convertable.get_width();
        int height = convertable.get_height();

        int y1i = get_first_y_position();
        int y2i = get_second_y_position();
        int ui = get_u_position();
        int vi = get_v_position();

        int packed_line_size = convertable.get_line_size(0);
        uint8_t *plane = convertable.get_modifiable_plane(0);
        const uint8_t *data = image.data;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j += 2) {
                int index = width * 4 * i + 4 * j;
                uint8_t rgb1[3] = {
                    data[index],
                    data[index + 1],
                    data[index + 2]
                };

                uint8_t rgb2[3] = {
                    data[index + 4],
                    data[index + 5],
                    data[index + 6]
                };

                uint8_t yuv_1[3], yuv_2[3];

                FAST_INV_CVT_601(rgb1, yuv_1);
                FAST_INV_CVT_601(rgb2, yuv_2);

                uint8_t *pixel_ptr = &plane[packed_line_size*i + j * 2];
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
