#define MAT(x, i, j) (x)[4 * (i) + (j)]
#include <iostream>
#include <algorithm>
#include "tsumaki-math.hpp"

namespace tsumaki {
    void inverse_4by4(const float *matrix, float *out) {
        float lhs[16], rhs[16];
        std::copy(matrix, matrix + 16, lhs);
        std::fill_n(rhs, 16, 0);
        MAT(rhs, 0, 0) = MAT(rhs, 1, 1) = MAT(rhs, 2, 2) = MAT(rhs, 3, 3) = 1;

        for (int i = 0; i < 4; i++) {
            float diag = MAT(lhs, i, i);

            for (int j = 0; j < 4; j++) {
                MAT(lhs, i, j) /= diag;
                MAT(rhs, i, j) /= diag;
            }

            for (int j = 0; j < 4; j++) {
                if (i == j) continue;
                float coef = MAT(lhs, j, i);
                for (int k = 0; k < 4; k++) {
                    MAT(lhs, j, k) -= coef * MAT(lhs, i, k);
                    MAT(rhs, j, k) -= coef * MAT(rhs, i, k);
                }
            }
        }
        std::copy(rhs, rhs + 16, out);
    }
}
