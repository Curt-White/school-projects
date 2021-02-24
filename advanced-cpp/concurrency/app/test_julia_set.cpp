#define CATCH_CONFIG_MAIN

#include "ra/julia_set.hpp"
#include "catch2/catch.hpp"
#include "boost/multi_array.hpp"
#include <chrono>

TEST_CASE("Compute Julia") {
	constexpr int width = 512;
	constexpr int height = 512;
	constexpr int max_iter = 255;
	constexpr int num_threads = 8;
	boost::multi_array<int, 2> a(boost::extents[height][width]);
	std::complex<double> bl(-1.25, -1.25), tr(1.25, 1.25), c(0.37, -0.16);

	// compute julia set and test timing
	auto t1 = std::chrono::high_resolution_clock::now();
	ra::fractal::compute_julia_set<double>(bl, tr, c, max_iter, a, num_threads);
	auto t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
	std::cout << "This program took: " << duration << " with " << num_threads << " threads" << std::endl;

	// print the data to a file
	std::ofstream myfile("/Users/curtwhite/Desktop/julia.pnm");
	if (myfile.is_open()) {
		myfile << "P2 " << width << " " << height << " " << max_iter << "\n";
		for (int row = height - 1; row >= 0; row--) {
			for (int col = 0; col < width; col++) {
				myfile << a[row][col];
				if (col != width - 1) myfile << " ";
			}

			myfile << "\n";
		}
	} else {
        std::cerr << "didn't write" << std::endl;
    }
}
