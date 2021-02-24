#include "ra/cexpr_basic_string.hpp"

// The 5 includes "P1", the 2 spaces and a newline character
// w * h is boxes, add h for new lines, add space for digits of W and H
#define COMP_SIZE(w, h) ((w * h) + h + 5 + ra::cexpr::num_digits(w) + ra::cexpr::num_digits(h))

namespace ra::fractal {
    constexpr int max_iterations = 16;

    constexpr double a0 = -1.6;
    constexpr double a1 = -1.1;
    constexpr double b0 = 0.6;
    constexpr double b1 = 1.1;

    template <std::size_t W, std::size_t H>
    constexpr int compute_cell(int k, int l) {
        double cre = a0 + k * ((b0 - a0) / (W - 1));
        double cim = a1 + (H - 1 - l) * ((b1 - a1) / (H - 1));

        double zre = 0;
        double zim = 0;
        for (int i = 0; i < max_iterations; i++) {
            if ((zre * zre + zim * zim) >= 4) {
                return 0;
            }

            double tmp = zre * zre - zim * zim + cre;
            zim = zre * zim + zim * zre + cim;
            zre = tmp;
        }

        return 1;
    }

    template <std::size_t W, std::size_t H>
    constexpr ra::cexpr::cexpr_basic_string<char, COMP_SIZE(W, H)> compute_mandelbrot() {
        constexpr std::size_t w_size = ra::cexpr::num_digits(W), h_size = ra::cexpr::num_digits(H);
        char init[5 + w_size + h_size + 1] = "P1 ";

        char *next = nullptr;
        ra::cexpr::to_string(W, &init[3], w_size + 1, &next);
        next[0] = ' ';
        ra::cexpr::to_string(H, &next[1], h_size + 1, &next);
        next[0] = '\n';
        next[1] = '\0';

        ra::cexpr::cexpr_basic_string<char, COMP_SIZE(W, H)> output(init);

        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                int pixel = compute_cell<W, H>(x, y);
                if (pixel == 1) {
                    output.append("1");
                } else {
                    output.append("0");
                }
            }

            output.append("\n");
        }

        return output;
    }

    template <std::size_t W, std::size_t H>
    constexpr auto mandelbrot = compute_mandelbrot<W, H>();
}