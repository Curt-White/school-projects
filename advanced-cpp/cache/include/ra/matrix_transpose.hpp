#include <array>
#include <iostream>

namespace ra::cache {

	template <class T>
	void recurse(const T* a, std::size_t m, std::size_t n, T* b, std::size_t orig_m, std::size_t orig_n) {
		if (m * n < 64) {
			for (int i = 0; i < m; i++) {
				for (int j = 0; j < n; j++) {
					*(b + orig_m * j + i) = *(a + orig_n * i + j);	
				}
			}
		} else {
			// if it is wider than it is tall or if it is square
			if (n >= m) {
				// then we split the A matrix vertically and B horizontally
				int c_vert = n / 2; // cols in a
				recurse(a, m, c_vert, b, orig_m, orig_n);	// left side
				recurse(a + c_vert, m, n - c_vert, b + (c_vert) * orig_m, orig_m, orig_n);	// right side
			} else { // if it is taller than it is wide
				// then we split the A matrix horizontally and B vertically
				int c_horz = m / 2; // cols in a
				recurse(a, c_horz, n, b, orig_m, orig_n);	// left side
				recurse(a + (c_horz) * orig_n, m - c_horz, n, b + c_horz, orig_m, orig_n);	// right side
			}
		}
	}

	// m rows and n columns
	template <class T>
	void matrix_transpose(const T* a, std::size_t m, std::size_t n, T* b) {
		if (a == nullptr || b == nullptr) return;
		// if a is equal to be create auxilliary buffer then copy back to a
		if (a == b) {
			auto temp_array = new T[m * n];
			recurse(a, m, n, temp_array, m, n);
			std::copy(temp_array, temp_array + n * m, b);
			delete[] temp_array;

			return;
		}
	
		recurse(a, m, n, b, m, n);
	}

	template <class T>
	void naive_matrix_transpose(const T* a, std::size_t m, std::size_t n, T* b) {
		if (a == nullptr || b == nullptr) return;
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				*(b + m * j + i) = *(a + n * i + j);
			}
		}
	}
}
