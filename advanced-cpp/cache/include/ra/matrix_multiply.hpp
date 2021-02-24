#include <array>
#include <iostream>

namespace ra::cache {
	template <class T>
	void multiply_recurse(const T* a, const T* b, std::size_t m, std::size_t n, std::size_t p, T* c, 
			std::size_t orig_m, std::size_t orig_n, std::size_t orig_p, bool accumulate) {
		
		if (m * n * p < 64) {
			for (int i = 0; i < m; ++i) {
				for (int j = 0; j < p; ++j) {
					T sum = T(0);
					for (int k = 0; k < n; ++k) {
						sum += *(a + orig_n * i + k) * *(b + k * orig_p + j);
					}

					// if accumulate is true then we add, else we assign
					if (accumulate) {
						*(c + i * orig_p + j) += sum;
					} else {
						*(c + i * orig_p + j) = sum;
					}
				}
			}
		} else {
			// if m is the greatest or if m == n and both are greater than p
			if ((m > n && m > p) || (m == n && m > p)) {
				int center_m = m / 2; // rows in a
				multiply_recurse(a, b, center_m, n, p, c, orig_m, orig_n, orig_p, accumulate);	// left side
				multiply_recurse((a + center_m * orig_n), b, (m - center_m), n, p, (c + center_m * orig_p), orig_m, orig_n, orig_p, accumulate);	// right side
			} else if ((n > m && n > p)) { // if n is the greatest dimension
				int center_n = n / 2; // col in a
				multiply_recurse(a, b, m, center_n, p, c, orig_m, orig_n, orig_p, accumulate);
				multiply_recurse(a + center_n, b + center_n * orig_p, m, (n - center_n), p, c, orig_m, orig_n, orig_p, true);
			} else { // if p is the greatest dimension or they are all equal
				int center_p = p / 2; // col in a
				multiply_recurse(a, b, m, n, center_p, c, orig_m, orig_n, orig_p, accumulate);	// left side
				multiply_recurse(a, b + center_p, m, n, (p - center_p), (c + center_p), orig_m, orig_n, orig_p, accumulate);	// right side
			}
		}
	}

	template <class T>
	void matrix_multiply(const T* a, const T* b, std::size_t m, std::size_t n, std::size_t p, T* c) {
		if (a == nullptr || b == nullptr || c == nullptr) return;
		multiply_recurse(a, b, m, n, p, c, m, n, p, false);
	}

	template <class T>
	void naive_matrix_multiply(const T* a, const T* b, std::size_t m, std::size_t n, std::size_t p, T* c) {
		if (a == nullptr || b == nullptr || c == nullptr) return;
		for (int i = 0; i < m; ++i) {
			for (int j = 0; j < p; ++j) {
				T sum = T(0);
				for (int k = 0; k < n; ++k) {
					sum += *(a + n * i + k) * *(b + k * p + j);
				}

				*(c + i * p + j) = sum;
			}
		}
	}
}
