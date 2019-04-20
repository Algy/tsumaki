#include "image-cvt.hpp"
#include "tsumaki-math.hpp"
#include <algorithm>

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

#ifdef USE_AVX2
#include <immintrin.h>
#endif 
#include <iostream>


namespace tsumaki {
    template <typename T>
    static inline T clamp(T val, T lo, T hi) {
        return (val < lo)? lo : ((val > hi)? hi : val);
    }

#ifdef USE_AVX2
#define QUANTIZE_LEVEL 7
#define BILINEAR_PROLOGUE  \
    int16_t *py_array = new int16_t[new_height]; \
    int16_t *px_array = new int16_t[new_width]; \
    int* x1_array = new int[new_width]; \
    int* x2_array = new int[new_width]; \
    int* y1_array = new int[new_height]; \
    int* y2_array = new int[new_height];\
    const int new_height_minus_1 = new_height - 1, new_width_minus_1 = new_width - 1;\
    for (int i = 0; i < new_height; i++) {\
        int widn_i = i * height;\
        int y1 = widn_i / new_height;\
        int y2 = (widn_i + new_height_minus_1) / new_height;\
        float py = (float)widn_i / new_height - y1;\
        y2 = (y2 >= height)? (height - 1) : y2;\
        py_array[i] = py * (1 << QUANTIZE_LEVEL);\
        y1_array[i] = y1;\
        y2_array[i] = y2;\
    }\
    for (int j = 0; j < new_width; j++) {\
        int widn_j = j * width;\
        int x1 = widn_j / new_width;\
        int x2 = (widn_j + new_width_minus_1) / new_width;\
        float px = (float)widn_j / new_width - x1;\
        x2 = (x2 >= width)? (width - 1) : x2;\
        px_array[j] = px * (1 << QUANTIZE_LEVEL);\
        x1_array[j] = x1;\
        x2_array[j] = x2;\
    }\
    __m256i max_255 = _mm256_set1_epi16(255);

#define BILINEAR_BODY(lt_vec, rt_vec, lb_vec, rb_vec) \
                __m256i top_sub = _mm256_sub_epi16(rt_vec, lt_vec);\
                __m256i bot_sub = _mm256_sub_epi16(rb_vec, lb_vec);\
                __m256i p1 = _mm256_add_epi16(lt_vec, _mm256_srai_epi16(_mm256_mullo_epi16(top_sub, interpx_vec), QUANTIZE_LEVEL));\
                __m256i p2 = _mm256_add_epi16(lb_vec, _mm256_srai_epi16(_mm256_mullo_epi16(bot_sub, interpx_vec), QUANTIZE_LEVEL));\
                __m256i p3 = _mm256_add_epi16(p1, _mm256_srai_epi16(_mm256_mullo_epi16(_mm256_sub_epi16(p2, p1), interpy_vec), QUANTIZE_LEVEL));\
                p3 = _mm256_min_epi16(p3, max_255);\
                __m128i lo = _mm256_extracti128_si256(p3, 0);\
                __m128i hi = _mm256_extracti128_si256(p3, 1);\
                __m128i pixs_vec = _mm_packus_epi16(lo, hi);

#define BILINEAR_EPILOGUE \
        delete [] py_array; \
        delete [] px_array; \
        delete [] x1_array; \
        delete [] x2_array; \
        delete [] y1_array; \
        delete [] y2_array;


    ConvertedMaskImage ConvertedMaskImage::resize_bilinear(int new_width, int new_height) {
        ConvertedMaskImage result(new_width, new_height);
        if (new_width <= 0 || new_height <= 0) return result;
        BILINEAR_PROLOGUE;
        const uint8_t* orig_data = data;
        uint8_t *new_data = result.data;
        for (int i = 0; i < new_height; i++) {
            __m256i interpy_vec = _mm256_set1_epi16(py_array[i]);
            int y1 = y1_array[i], y2 = y2_array[i];
            for (int j = 0; j < new_width; j += 16) {
                int x1[16], x2[16];
                std::copy(x1_array + j, x1_array + j + 16, x1);
                std::copy(x2_array + j, x2_array + j + 16, x2);

                int16_t lt_vec_arr[16], rt_vec_arr[16], lb_vec_arr[16], rb_vec_arr[16];

                for (int k =0; k < 16; k++) {
                    lt_vec_arr[k] = orig_data[width * y1 + x1[k]];
                    rt_vec_arr[k] = orig_data[width * y1 + x2[k]];
                    lb_vec_arr[k] = orig_data[width * y2 + x1[k]];
                    rb_vec_arr[k] = orig_data[width * y2 + x2[k]];
                }

                __m256i interpx_vec = _mm256_loadu_si256((__m256i *)&px_array[j]);  // load 16 pixels
                __m256i lt_vec = _mm256_loadu_si256((__m256i *)&lt_vec_arr);
                __m256i rt_vec = _mm256_loadu_si256((__m256i *)&rt_vec_arr);
                __m256i lb_vec = _mm256_loadu_si256((__m256i *)&lb_vec_arr);
                __m256i rb_vec = _mm256_loadu_si256((__m256i *)&rb_vec_arr);
                BILINEAR_BODY(lt_vec, rt_vec, lb_vec, rb_vec);
                _mm_storeu_si128((__m128i*)&new_data[new_width * i + j], pixs_vec);
            }
        }
        BILINEAR_EPILOGUE;
        return result;
    }

    ConvertedRGBAImage ConvertedRGBAImage::resize_bilinear(int new_width, int new_height) {
        ConvertedRGBAImage result(new_width, new_height);
        if (new_width <= 0 || new_height <= 0) return result;

        BILINEAR_PROLOGUE
        const uint8_t* orig_data = data;
        uint8_t *new_data = result.data;

        __m256i hi_word_dropper = _mm256_set1_epi16(0x00FF);
        __m256i duplicate_mask = _mm256_set_epi32(
            3, 3, 2, 2,
            1, 1, 0, 0
        );
        for (int i = 0; i < new_height; i++) {
            __m256i interpy_vec = _mm256_set1_epi16(py_array[i]);
            int y1 = y1_array[i], y2 = y2_array[i];
            for (int j = 0; j < new_width; j += 4) {
                int x1[4] = {x1_array[j], x1_array[j+1], x1_array[j+2], x1_array[j+3]};
                int x2[4] = {x2_array[j], x2_array[j + 1], x2_array[j + 2], x2_array[j + 3]};

                __m128i m = _mm_loadl_epi64((__m128i *)&px_array[j]); 
                __m128i dup_m = _mm_unpacklo_epi16(m, m); // [px_array[j], px_array[j], px_array[j+1], px_array[j+1], px_array[j+2], px_array[j+2], px_array[j+3], px_array[j+3]
                __m256i interpx_vec = _mm256_permutevar8x32_epi32(_mm256_castsi128_si256(dup_m), duplicate_mask);


                __m256i lt_vec = _mm256_cvtepi8_epi16(_mm_set_epi32(
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x1[3]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x1[2]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x1[1]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x1[0]]
                    )
                );
                __m256i rt_vec = _mm256_cvtepi8_epi16(_mm_set_epi32(
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x2[3]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x2[2]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x2[1]],
                        *(int32_t*)&orig_data[4 * width * y1 + 4 * x2[0]]
                    )
                );
                __m256i lb_vec = _mm256_cvtepi8_epi16(_mm_set_epi32(
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x1[3]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x1[2]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x1[1]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x1[0]]
                    )
                );
                __m256i rb_vec = _mm256_cvtepi8_epi16(_mm_set_epi32(
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x2[3]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x2[2]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x2[1]],
                        *(int32_t*)&orig_data[4 * width * y2 + 4 * x2[0]]
                    )
                );
                lt_vec = _mm256_and_si256(lt_vec, hi_word_dropper);
                rt_vec = _mm256_and_si256(rt_vec, hi_word_dropper);
                lb_vec = _mm256_and_si256(lb_vec, hi_word_dropper);
                rb_vec = _mm256_and_si256(rb_vec, hi_word_dropper);
                BILINEAR_BODY(lt_vec, rt_vec, lb_vec, rb_vec);
                _mm_storeu_si128((__m128i*)&new_data[4 * new_width * i + 4 * j], pixs_vec);
            }
        }
        BILINEAR_EPILOGUE;

        return result;
    }
#else
    ConvertedMaskImage ConvertedMaskImage::resize_bilinear(int new_width, int new_height) {
        ConvertedMaskImage result(new_width, new_height);
        // TODO
        return result;
    }
    ConvertedRGBAImage ConvertedRGBAImage::resize_bilinear(int new_width, int new_height) {
        ConvertedRGBAImage result(new_width, new_height);
        if (new_width <= 0 || new_height <= 0) return result;

        float *py_array = new float[new_height];
        float *px_array = new float[new_width];
        int* x1_array = new int[new_width];
        int* x2_array = new int[new_width];
        int* y1_array = new int[new_height];
        int* y2_array = new int[new_height];

        const int new_height_minus_1 = new_height - 1, new_width_minus_1 = new_width - 1;
        for (int i = 0; i < new_height; i++) {
            int widn_i = i * height;
            int y1 = widn_i / new_height;
            int y2 = (widn_i + new_height_minus_1) / new_height;
            float py = (float)widn_i / new_height - y1;
            y2 = (y2 >= height)? (height - 1) : y2;
            py_array[i] = py;
            y1_array[i] = y1;
            y2_array[i] = y2;
        }

        for (int j = 0; j < new_width; j++) {
            int widn_j = j * width;
            int x1 = widn_j / new_width;
            int x2 = (widn_j + new_width_minus_1) / new_width;
            float px = (float)widn_j / new_width - x1;
            x2 = (x2 >= width)? (width - 1) : x2;
            px_array[j] = px;
            x1_array[j] = x1;
            x2_array[j] = x2;
        }

        const uint8_t* orig_data = data;
        uint8_t *new_data = result.data;

        int st_i = 0;

        for (int i = st_i; i < new_height; i++) {
            int y1 = y1_array[i], y2 = y2_array[i];
            float interp_y = py_array[i];
            for (int j = 0; j < new_width; j++) {
                int x1 = x1_array[j], x2 = x2_array[j];
                float interp_x = px_array[j];
                float lts[4], rts[4], lbs[4], rbs[4];
                float p1[4], p2[4], p3[4];
                for (int k = 0; k < 4; k++) {
                    lts[k] = (float)orig_data[4 * width * y1 + 4 * x1 + k];
                }

                for (int k = 0; k < 4; k++) {
                    rts[k] = (float)orig_data[4 * width * y1 + 4 * x2 + k];
                }
                for (int k = 0; k < 4; k++) {
                    lbs[k] = (float)orig_data[4 * width * y2 + 4 * x1 + k];
                }
                for (int k = 0; k < 4; k++) {
                    rbs[k] = (float)orig_data[4 * width * y2 + 4 * x2 + k];
                }

                for (int k = 0; k < 4; k++) {
                    p1[k] = lts[k] + (rts[k] - lts[k]) * interp_x;
                    p2[k] = lbs[k] + (lbs[k] - rbs[k]) * interp_x;
                    p3[k] = p1[k] + (p2[k] - p1[k]) * interp_y;
                }

                for (int k = 0; k < 4; k++) {
                    new_data[4 * new_width * i + 4 * j + k] = (uint8_t)clamp(p3[k], 0.0f, 255.0f);
                }
            }
        }

        delete [] py_array;
        delete [] px_array;
        delete [] x1_array;
        delete [] x2_array;
        delete [] y1_array;
        delete [] y2_array;
        return result;
    }
#endif

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

#ifdef USE_AVX2
#define AVX_CVT_COLOR_PRELUDE(color_matrix) \
    __m256 w11 = _mm256_broadcast_ss(&color_matrix[0]), w12 = _mm256_broadcast_ss(&color_matrix[1]), w13 = _mm256_broadcast_ss(&color_matrix[2]); \
    __m256 w21 = _mm256_broadcast_ss(&color_matrix[4]), w22 = _mm256_broadcast_ss(&color_matrix[5]), w23 = _mm256_broadcast_ss(&color_matrix[6]); \
    __m256 w31 = _mm256_broadcast_ss(&color_matrix[8]), w32 = _mm256_broadcast_ss(&color_matrix[9]), w33 = _mm256_broadcast_ss(&color_matrix[10]); \
    float constants[3] = {color_matrix[3] * 255, color_matrix[7] * 255, color_matrix[11] * 255}; \
    __m256 min_zero = _mm256_set1_ps(0.0f), max_255 = _mm256_set1_ps(255.0f);

#define AVX_CVT_BODY(Y, U, V, R, G, B) \
    __m256 r_line = _mm256_mul_ps(Y, w11); \
    __m256 g_line = _mm256_mul_ps(Y, w21); \
    __m256 b_line = _mm256_mul_ps(Y, w31); \
    __m256 rt = _mm256_mul_ps(U, w12); \
    __m256 gt = _mm256_mul_ps(U, w22); \
    __m256 bt = _mm256_mul_ps(U, w32); \
    r_line = _mm256_add_ps(r_line, rt); \
    rt = _mm256_mul_ps(V, w13); \
    g_line = _mm256_add_ps(g_line, gt); \
    gt = _mm256_mul_ps(V, w23); \
    b_line = _mm256_add_ps(b_line, bt); \
    bt = _mm256_mul_ps(V, w33); \
    r_line = _mm256_add_ps(r_line, rt); \
    g_line = _mm256_add_ps(g_line, gt); \
    b_line = _mm256_add_ps(b_line, bt); \
    rt = _mm256_broadcast_ss(&constants[0]); \
    gt = _mm256_broadcast_ss(&constants[1]); \
    bt = _mm256_broadcast_ss(&constants[2]); \
    r_line = _mm256_add_ps(r_line, rt); \
    g_line = _mm256_add_ps(g_line, gt); \
    b_line = _mm256_add_ps(b_line, bt); \
    r_line = _mm256_max_ps(r_line, min_zero); \
    g_line = _mm256_max_ps(g_line, min_zero); \
    b_line = _mm256_max_ps(b_line, min_zero); \
    r_line = _mm256_min_ps(r_line, max_255); \
    g_line = _mm256_min_ps(g_line, max_255); \
    b_line = _mm256_min_ps(b_line, max_255); \
    float R[8], G[8], B[8]; \
    _mm256_storeu_ps(R, r_line); \
    _mm256_storeu_ps(G, g_line); \
    _mm256_storeu_ps(B, b_line);


#define TO_UINT8(x) (uint8_t)(x)

#endif 

    shared_ptr<ConvertedRGBAImage> I420FormatCvt::convert() {
        int width = convertable.get_width();
        int height = convertable.get_height();
        shared_ptr<ConvertedRGBAImage> result(new ConvertedRGBAImage(width, height));
        int y_line_size = convertable.get_line_size(0);
        int u_line_size = convertable.get_line_size(1);

        const uint8_t* planes[3] { convertable.get_plane(0), convertable.get_plane(1), convertable.get_plane(2) };
        const uint8_t *y_plane = planes[0];
        uint8_t *data = result->data;

#ifdef USE_AVX2
        // Chunk
        // ------------------------------
        // y11 y12 y13 y14
        // u1   u1  u2  u2 
        // v1   v1  v1  v2 
        // -------------------------------
        // y21 y22 y23 y24
        // u1   u1  u2  u2 
        // v1   v1  v2  v2
        // 
        // R = w11 * Y + w12 * U + w13 * V + w14
        // G = w21 * Y + w22 * U + w23 * V + w24
        // B = w31 * Y + w32 * U + w33 * V + w34
        const float *color_matrix = convertable.get_color_matrix();
        AVX_CVT_COLOR_PRELUDE(color_matrix);
        for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width / 2; j += 2) {
                int chroma_index = u_line_size * i + j;
                int y_base = y_line_size * 2 * i + 2 * j;
                float u1 = planes[1][chroma_index], u2 = planes[1][chroma_index + 1];
                float v1 = planes[2][chroma_index], v2 = planes[2][chroma_index + 1];

                __m256 Y = _mm256_set_ps(
                    (float)y_plane[y_base + y_line_size + 3],
                    (float)y_plane[y_base + y_line_size + 2],
                    (float)y_plane[y_base + y_line_size + 1],
                    (float)y_plane[y_base + y_line_size],
                    (float)y_plane[y_base + 3],
                    (float)y_plane[y_base + 2],
                    (float)y_plane[y_base + 1],
                    (float)y_plane[y_base]
                );
                __m256 U = _mm256_set_ps(
                    u2,
                    u2,
                    u1,
                    u1,
                    u2,
                    u2,
                    u1,
                    u1
                );

                __m256 V = _mm256_set_ps(
                    v2,
                    v2,
                    v1,
                    v1,
                    v2,
                    v2,
                    v1,
                    v1
                );
                AVX_CVT_BODY(Y, U, V, R, G, B);

                int index = 4 * width * 2 * i + 4 * 2 * j;
                data[index + 0] = TO_UINT8(R[0]);
                data[index + 1] = TO_UINT8(G[0]);
                data[index + 2] = TO_UINT8(B[0]);

                data[index + 4] = TO_UINT8(R[1]);
                data[index + 5] = TO_UINT8(G[1]);
                data[index + 6] = TO_UINT8(B[1]);

                data[index + 8] = TO_UINT8(R[2]);
                data[index + 9] = TO_UINT8(G[2]);
                data[index + 10] = TO_UINT8(B[2]);

                data[index + 12] = TO_UINT8(R[3]);
                data[index + 13] = TO_UINT8(G[3]);
                data[index + 14] = TO_UINT8(B[3]);

                data[index + 4 * width + 0] = TO_UINT8(R[4]);
                data[index + 4 * width + 1] = TO_UINT8(G[4]);
                data[index + 4 * width + 2] = TO_UINT8(B[4]);

                data[index + 4 * width + 4] = TO_UINT8(R[5]);
                data[index + 4 * width + 5] = TO_UINT8(G[5]);
                data[index + 4 * width + 6] = TO_UINT8(B[5]);

                data[index + 4 * width + 8] = TO_UINT8(R[6]);
                data[index + 4 * width + 9] = TO_UINT8(G[6]);
                data[index + 4 * width + 10] = TO_UINT8(B[6]);

                data[index + 4 * width + 12] = TO_UINT8(R[7]);
                data[index + 4 * width + 13] = TO_UINT8(G[7]);
                data[index + 4 * width + 14] = TO_UINT8(B[7]);
            }
        }
#else
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
#endif
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
#ifdef USE_AVX2
        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);
        AVX_CVT_COLOR_PRELUDE(inv_color_matrix);
        for (int i = 0; i < height / 2; i++) {
            for (int j = 0; j < width / 2; j += 2) {
                int y_base = y_line_size * 2 * i + 2 * j;
                int chroma_index = u_line_size * i + j;
                int index = 4 * width * 2 * i + 4 * 2 * j;
                float rgb[3][8] = {
                    { (float)data[index], (float)data[index + 4], (float)data[index + 8], (float)data[index + 12],
                      (float)data[index + 4 * width], (float)data[index + 4 * width + 4], (float)data[index + 4 * width + 8], (float)data[index + 4 * width + 12] },
                    { (float)data[index + 1], (float)data[index + 5], (float)data[index + 9], (float)data[index + 13],
                      (float)data[index + 4 * width + 1], (float)data[index + 4 * width + 5], (float)data[index + 4 * width + 9], (float)data[index + 4 * width + 13] },
                    { (float)data[index + 2], (float)data[index + 6], (float)data[index + 10], (float)data[index + 14],
                      (float)data[index + 4 * width + 2], (float)data[index + 4 * width + 6], (float)data[index + 4 * width + 10], (float)data[index + 4 * width + 14] }
                };

                __m256 R = _mm256_loadu_ps(rgb[0]), G = _mm256_loadu_ps(rgb[1]), B = _mm256_loadu_ps(rgb[2]);
                AVX_CVT_BODY(R, G, B, Y, U, V);
                plane_0[y_base] = TO_UINT8(Y[0]);
                plane_0[y_base + 1] = TO_UINT8(Y[1]);
                plane_0[y_base + 2] = TO_UINT8(Y[2]);
                plane_0[y_base + 3] = TO_UINT8(Y[3]);
                plane_0[y_base + y_line_size] = TO_UINT8(Y[4]);
                plane_0[y_base + y_line_size + 1] = TO_UINT8(Y[5]);
                plane_0[y_base + y_line_size + 2] = TO_UINT8(Y[6]);
                plane_0[y_base + y_line_size + 3] = TO_UINT8(Y[7]);

                plane_1[chroma_index] = TO_UINT8(((U[0] + U[1] + U[4] + U[5]) / 4.0f));
                plane_1[chroma_index + 1] = TO_UINT8(((U[2] + U[3] + + U[6] + U[7]) / 4.0f));

                plane_2[chroma_index] = TO_UINT8(((V[0] + V[1] + V[4] + V[5]) / 4.0f));
                plane_2[chroma_index + 1] = TO_UINT8(((V[2] + V[3] + V[6] + V[7]) / 4.0f));
            }
        }
#else
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
#endif
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

#ifdef USE_AVX2
        const float *color_matrix = convertable.get_color_matrix();
        AVX_CVT_COLOR_PRELUDE(color_matrix);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width * 2; j += 16) {
                const uint8_t *pixel_ptr = &plane[packed_line_size*i + j];

                __m256 Y = _mm256_set_ps(
                    pixel_ptr[12 + y2i],
                    pixel_ptr[12 + y1i],
                    pixel_ptr[8 + y2i],
                    pixel_ptr[8 + y1i],
                    pixel_ptr[4 + y2i],
                    pixel_ptr[4 + y1i],
                    pixel_ptr[y2i],
                    pixel_ptr[y1i]
                );
                __m256 U = _mm256_set_ps(
                    pixel_ptr[12 + ui],
                    pixel_ptr[12 + ui],
                    pixel_ptr[8 + ui],
                    pixel_ptr[8 + ui],
                    pixel_ptr[4 + ui],
                    pixel_ptr[4 + ui],
                    pixel_ptr[ui],
                    pixel_ptr[ui]
                );

                __m256 V = _mm256_set_ps(
                    pixel_ptr[12 + vi],
                    pixel_ptr[12 + vi],
                    pixel_ptr[8 + vi],
                    pixel_ptr[8 + vi],
                    pixel_ptr[4 + vi],
                    pixel_ptr[4 + vi],
                    pixel_ptr[vi],
                    pixel_ptr[vi]
                );
                AVX_CVT_BODY(Y, U, V, R, G, B);

                int index = width * 4 * i + 4 * (j / 2);
                for (int k = 0; k < 8; k++) {
                    data[index + 4 * k] = TO_UINT8(R[k]);
                    data[index + 4 * k + 1] = TO_UINT8(G[k]);
                    data[index + 4 * k + 2] = TO_UINT8(B[k]);
                }
            }
        }
#else
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
#endif
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
#ifdef USE_AVX2
        float inv_color_matrix[16];
        convertable.get_inverse_color_matrix(inv_color_matrix);

        AVX_CVT_COLOR_PRELUDE(inv_color_matrix);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j += 8) {
                int index = width * 4 * i + 4 * j;

                __m256 R = _mm256_set_ps(
                    data[index + 28],
                    data[index + 24],
                    data[index + 20],
                    data[index + 16],
                    data[index + 12],
                    data[index + 8],
                    data[index + 4],
                    data[index]
                );

                __m256 G = _mm256_set_ps(
                    data[index + 29],
                    data[index + 25],
                    data[index + 21],
                    data[index + 17],
                    data[index + 13],
                    data[index + 9],
                    data[index + 5],
                    data[index + 1]
                );

                __m256 B = _mm256_set_ps(
                    data[index + 30],
                    data[index + 26],
                    data[index + 22],
                    data[index + 18],
                    data[index + 14],
                    data[index + 10],
                    data[index + 6],
                    data[index + 2]
                );

                AVX_CVT_BODY(R, G, B, Y, U, V);

                uint8_t *pixel_ptr = &plane[packed_line_size*i + j * 2];
                for (int k = 0; k < 4; k++) {
                    pixel_ptr[4 * k + y1i] = TO_UINT8(Y[2 * k + 0]);
                    pixel_ptr[4 * k + y2i] = TO_UINT8(Y[2 * k + 1]);
                    pixel_ptr[4 * k + ui] = TO_UINT8((U[2 * k + 0] + U[2 * k + 1]) / 2.0f);
                    pixel_ptr[4 * k + vi] = TO_UINT8((V[2 * k + 0] + V[2 * k + 1]) / 2.0f);
                }
            }
        }
#else
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
#endif
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
        const uint8_t* planes[3] { convertable.get_plane(0), convertable.get_plane(1), convertable.get_plane(2) };
        const float *color_matrix = convertable.get_color_matrix();
#if USE_AVX2
        AVX_CVT_COLOR_PRELUDE(color_matrix);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j += 8) {
                __m256 Y = _mm256_set_ps(
                    planes[0][width*i + j + 7],
                    planes[0][width*i + j + 6],
                    planes[0][width*i + j + 5],
                    planes[0][width*i + j + 4],
                    planes[0][width*i + j + 3],
                    planes[0][width*i + j + 2],
                    planes[0][width*i + j + 1],
                    planes[0][width*i + j]
                );
                __m256 U = _mm256_set_ps(
                    planes[1][width*i + j + 7],
                    planes[1][width*i + j + 6],
                    planes[1][width*i + j + 5],
                    planes[1][width*i + j + 4],
                    planes[1][width*i + j + 3],
                    planes[1][width*i + j + 2],
                    planes[1][width*i + j + 1],
                    planes[1][width*i + j]
                );
                __m256 V = _mm256_set_ps(
                    planes[2][width*i + j + 7],
                    planes[2][width*i + j + 6],
                    planes[2][width*i + j + 5],
                    planes[2][width*i + j + 4],
                    planes[2][width*i + j + 3],
                    planes[2][width*i + j + 2],
                    planes[2][width*i + j + 1],
                    planes[2][width*i + j]
                );
                AVX_CVT_BODY(Y, U, V, R, G, B);
                for (int k = 0; k < 8; k++) {
                    result->set_pixel(i, j, R[k], G[k], B[k], 255);
                }
            }
        }
#else
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
#endif
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
