#include <array>
#include <cmath>
#include "ra/matrix_transpose.hpp"
#include "boost/math/constants/constants.hpp"
#include <iostream>

namespace ra::cache {
	template <class T> void forward_fft(T* x, std::size_t n);

	std::tuple<int, int> almost_squares(int in) {
		int sqr_root = (int)sqrt(in);
		while (in % sqr_root) --sqr_root;
		return {sqr_root, in / sqr_root};
	}

	/**
	 * x: is the array of values being passed into the function
	 * n: is the size of the array being passed in, n should be a power of 2
	 */
	template <class T>
	void forward_fft(T* x, std::size_t n) {
		if (x == nullptr) return;

		if (n == 1) {
			return;
		} if (n == 2) {
			auto x0 = x[0], x1 = x[1];
			x[0] = x0 + x1;
			x[1] = x0 - x1;
		} else if (n == 4) {
			auto x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3];
			auto i = T(0, 1);
			x[0] = x0 + x1 + x2 + x3;
			x[1] = x0 - x2 + i * (-x1 + x3);
			x[2] = x0 - x1 + x2 - x3;
			x[3] = x0 - x2 + i * (x1 - x3);
		} else {
			// find the factors of n
			auto [ n1, n2 ] = almost_squares(n);
			assert(n1 * n2 == n);

			// transpose to n2 x n1 array
			auto temp_array = new T[n];
			matrix_transpose(x, n1, n2, temp_array);
			std::copy(temp_array, temp_array + n, x);
			delete[] temp_array;

			for (int i = 0; i < n2; i++) forward_fft((x + i * n1), n1);
	
			for (int row = 0; row < n2; row++) {
				for (int col = 0; col < n1; col++) {
					auto exp = T(0, -2.0*row*col*boost::math::constants::pi<double>() / n);
					T val = std::pow(T(boost::math::constants::e<double>()), exp);
					x[row * n1 + col] *= val;
				}
			}

			// transpose back to n1 x n2 matrix
			temp_array = new T[n];
			matrix_transpose(x, n2, n1, temp_array);
			std::copy(temp_array, temp_array + n, x);
			delete[] temp_array;

			// replace each row by dft for each n1 rows
			for (int  i = 0; i < n1; i++) forward_fft((x + i * n2), n2);

			// transpose back to n2 x n1 matrix
			temp_array = new T[n];
			matrix_transpose(x, n1, n2, temp_array);
			std::copy(temp_array, temp_array + n, x);
			delete[] temp_array;
		}
	}
}
