#define CATCH_CONFIG_MAIN

#include "ra/matrix_multiply.hpp"
#include <catch2/catch.hpp>
#include <iomanip>
#include <stdlib.h>
#include <time.h>

template <class T>
void print_matrix(const T* a, int m, int n) {
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			std::cout << std::setw(5) << a[i][j] << " ";
		}

		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template <class T>
bool comp_matrix(const T* a, const T* b, int m, int n) {
	for (int i = 0; i < m * n; i++) {
		if (a[i] != b[i]) return false;
	}

	return true;
}

template <class T>
void fill_matrix(T* a, int len) {
	srand (time(NULL));
	for (int i = 0; i < len; i++) {
		a[i] = rand() % 10 + 1;
	}
}

template <int M, int N, int P>
bool test_by_size() {
	long a[M][N];
	long b[N][P];
	long naive_c[M][P];
	long recur_c[M][P];

	fill_matrix(a[0], M * N);
	fill_matrix(b[0], N * P);

	ra::cache::naive_matrix_multiply(a[0], b[0], M, N, P, naive_c[0]);
	ra::cache::matrix_multiply(a[0], b[0], M, N, P, recur_c[0]);

	return comp_matrix(naive_c[0], recur_c[0], M, P);
}

TEST_CASE("random") {
	// smallest possible
	REQUIRE(test_by_size<1, 1, 1>());
	// random under 64 n*p*m
	REQUIRE(test_by_size<2, 4, 6>());
	// extra wide matrix so n is biggest dim
	REQUIRE(test_by_size<4, 36, 10>);
	// extra tall so m is biggest dim
	REQUIRE(test_by_size<45, 23, 34>());
	// extra tall so m is biggest dim
	REQUIRE(test_by_size<23, 12, 34>());
	// really big matrix
	REQUIRE(test_by_size<123, 112, 137>());
	// really big matrix
	REQUIRE(test_by_size<23, 3, 7654>());
	// really big matrix
	REQUIRE(test_by_size<1, 23, 4444>());
	// really big matrix
	REQUIRE(test_by_size<23, 234, 54>());
	// really big matrix
	REQUIRE(test_by_size<55, 55, 55>());
	// really big matrix
	REQUIRE(test_by_size<44, 44, 44>());
}
