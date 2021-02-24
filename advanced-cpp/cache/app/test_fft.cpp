#define CATCH_CONFIG_MAIN

#include "ra/fft.hpp"
#include <catch2/catch.hpp>

template <class T>
void print_matrix(const T* a, int m, int n) {
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			std::cout << std::setw(5) << a[i * n + j] << " ";
		}

		std::cout << std::endl;
	}
	std::cout << std::endl;
}

TEST_CASE("Null Array") {
	std::complex<double> *arr = nullptr;
	ra::cache::forward_fft(arr, 0);
}

TEST_CASE("Random 2") {
	constexpr int size = 2;
	std::complex<double> arr[size] = { 
		std::complex<double>(2), 
		std::complex<double>(6), 
	};

	// print_matrix(arr, 2, 2);
	ra::cache::forward_fft(arr, size);
	print_matrix(arr, 1, size);
}

TEST_CASE("Random 4") {
	constexpr int size = 4;
	std::complex<double> arr[size] = { 
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4)
	};

	// print_matrix(arr, 2, 2);
	ra::cache::forward_fft(arr, size);
	print_matrix(arr, 1, size);
}

TEST_CASE("Random 8") {
	constexpr int size = 8;
	std::complex<double> arr[size] = { 
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4),
		std::complex<double>(4), 
		std::complex<double>(65), 
		std::complex<double>(32), 
		std::complex<double>(14)
	};

	// print_matrix(arr, 2, 2);
	ra::cache::forward_fft(arr, size);
	print_matrix(arr, 1, size);
}

TEST_CASE("Random 32") {
	constexpr int size = 32;
	std::complex<double> arr[size] = { 
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4),
		std::complex<double>(4), 
		std::complex<double>(65), 
		std::complex<double>(32), 
		std::complex<double>(14),
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4),
		std::complex<double>(4), 
		std::complex<double>(65), 
		std::complex<double>(32), 
		std::complex<double>(14),
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4),
		std::complex<double>(4), 
		std::complex<double>(65), 
		std::complex<double>(32), 
		std::complex<double>(14),
		std::complex<double>(2), 
		std::complex<double>(6), 
		std::complex<double>(3), 
		std::complex<double>(4),
		std::complex<double>(4), 
		std::complex<double>(65), 
		std::complex<double>(32), 
		std::complex<double>(14)
	};

	ra::cache::forward_fft(arr, size);
	print_matrix(arr, 1, size);
}
