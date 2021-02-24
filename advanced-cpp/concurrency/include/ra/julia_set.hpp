#include "boost/multi_array.hpp"
#include "ra/thread_pool.hpp"
#include <iostream>

namespace ra::fractal {
	template <class Real>
	void compute_julia_set(const std::complex<Real>& bottom_left,
		const std::complex<Real>& top_right, const std::complex<Real>& c,
		int max_iters, boost::multi_array<int, 2>& a, int num_threads) {
		
		assert(num_threads >= 1);	
		ra::concurrency::thread_pool pool(num_threads);

		// loop through all of the rows
		for (int row = 0; row < a.shape()[0]; row++) {
			pool.schedule([&top_right, &bottom_left, &c, &a, max_iters, row] () {
				for (int col = 0; col < a.shape()[1]; col++) {
					Real ncol(col), nrow(row), hm1(a.shape()[0] - 1), wm1(a.shape()[1] - 1);
					std::complex<Real> z0 = std::complex<Real>(
						bottom_left.real() + (ncol / wm1) * (top_right.real() - bottom_left.real()),
						bottom_left.imag() + (nrow / hm1) * (top_right.imag() - bottom_left.imag())
					);

					// iterate until magnitude is greater than 0
					std::complex<Real> zn;
					int n;
					for (n = 0; n <= max_iters; n++) {
						zn = (n == 0 ? z0 : zn * zn + c);
						if (std::abs(zn) > 2) break;
					}

					a[row][col] = n;
				}
			});
		}

		pool.shutdown();
	}
}
