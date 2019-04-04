#include "frame.hpp"

namespace tsumaki {
    const float identity_color_matrix[16] {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    const float* RGBAFrame::get_color_matrix() const { return identity_color_matrix; };
}
