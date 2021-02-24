#define CATCH_CONFIG_MAIN

#include "ra/matrix_transpose.hpp"
#include <catch2/catch.hpp>
#include <iomanip>

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

template <class T>
bool comp_matrix(const T* a, const T* b, int m, int n) {
	for (int i = 0; i < m * n; i++) {
		// std::cout << i << " " << a[i] << " " << b[i] << std::endl;
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

template <int M, int N>
bool test_by_size() {
	long a[M][N];
	long naive_c[N][M];
	long recur_c[N][M];

	fill_matrix(a[0], M * N);

	ra::cache::matrix_transpose(a[0], M, N, recur_c[0]);
	ra::cache::naive_matrix_transpose(a[0], M, N, naive_c[0]);

	return comp_matrix(naive_c[0], recur_c[0], N, M);
}

TEST_CASE("A is equal to B") {
	int M = 10;
	int N = 13;
	long a[M][N];
	long naive_c[N][M];
	long recur_c[N][M];

	fill_matrix(a[0], M * N);

	ra::cache::naive_matrix_transpose(a[0], M, N, naive_c[0]);
	ra::cache::matrix_transpose(a[0], M, N, a[0]);

	REQUIRE(comp_matrix(naive_c[0], a[0], N, M));
}

TEST_CASE("actual random") {
	// smallest possible
	REQUIRE(test_by_size<1, 1>());
	// random under 64 n*p*m
	REQUIRE(test_by_size<2, 4>());
	// extra wide matrix so n is biggest dim
	REQUIRE(test_by_size<4, 36>);
	// extra tall so m is biggest dim
	REQUIRE(test_by_size<451, 22>());
	// extra tall so m is biggest dim
	REQUIRE(test_by_size<23, 12>());
	// really big matrix
	REQUIRE(test_by_size<123, 112>());
	// really big matrix
	REQUIRE(test_by_size<23, 7654>());
	// really big matrix
	REQUIRE(test_by_size<100, 444>());
	// really big matrix
	REQUIRE(test_by_size<23, 234>());
	// really big matrix
	REQUIRE(test_by_size<55, 55>());
	// really big matrix
	REQUIRE(test_by_size<44, 44>());
}

TEST_CASE("random") {
	int test_1[8][10] = { { 34, 56, 6, 76, 22, 654, 123, 87, 66, 6 },
					 { 23, 45, 567, 687, 34, 64, 33, 666, 23, 12 }, 
					 { 24, 777, 345, 987, 1, 5, 34, 678, 546, 0 },
					 { 34, 67 ,345, 765, 765, 234, 123, 6, 38, 8 },
					 { 1, 5, 6, 7, 5, 3, 5, 9, 0, 3 },
					 { 174, 456, 457, 342, 3, 56, 76, 43, 45, 1233 },
					 { 345, 567, 345, 455, 234, 876, 654, 543, 234, 356 },
					 { 346, 768, 45, 34, 345 , 234 , 467, 678, 789, 234 }
					};
	double test_2[1][1] = { 1 };
	double test_3[10][8] = {  34, 56, 6, 76, 22, 654, 123, 87, 66, 6 ,
					  23, 45, 567, 687, 34, 64, 33, 666, 23, 12 , 
					  24, 777, 345, 987, 1, 5, 34, 678, 546, 0 ,
					  34, 67 ,345, 765, 765, 234, 123, 6, 38, 8 ,
					  1, 5, 6, 7, 5, 3, 5, 9, 0, 3 ,
					  174, 456, 457, 342, 3, 56, 76, 43, 45, 789 ,
					  345, 567, 345, 455, 234, 876, 654, 543, 234, 356 ,
					  346, 768, 789, 34, 345 , 234 , 467, 678, 789, 234 
					};

	double test_4[9][7] = {  34, 56, 6, 76, 22, 654, 123, 87, 66,
					23, 45, 567, 687, 34, 64, 33, 666, 23, 
					24, 777, 345, 987, 1, 5, 34, 678, 546,
					34, 67 ,345, 765, 765, 234, 123, 6, 38,
					1, 5, 6, 7, 5, 3, 5, 9, 0,
					174, 456, 457, 342, 3, 56, 76, 43, 45,
					345, 567, 345, 455, 234, 876, 654, 543, 234,
				};


	// wide matrix
	int t1_out_rec[10][8];
	int t1_out_nieve[10][8];
	ra::cache::matrix_transpose(test_1[0], 8, 10, t1_out_rec[0]);
	ra::cache::naive_matrix_transpose(test_1[0], 8, 10, t1_out_nieve[0]);
	REQUIRE(comp_matrix(t1_out_nieve[0], t1_out_rec[0], 10, 8));

	// small matrix
	double t2_out_rec[1][1];
	double t2_out_nieve[1][1];
	ra::cache::matrix_transpose(test_2[0], 1, 1, t2_out_rec[0]);
	ra::cache::naive_matrix_transpose(test_2[0], 1, 1, t2_out_nieve[0]);
	REQUIRE(comp_matrix(t2_out_rec[0], t2_out_nieve[0], 1, 1));

	// tall matrix
	double t3_out_rec[8][10];
	double t3_out_nieve[8][10];
	ra::cache::matrix_transpose(test_3[0], 10, 8, t3_out_rec[0]);
	ra::cache::naive_matrix_transpose(test_3[0], 10, 8, t3_out_nieve[0]);
	REQUIRE(comp_matrix(t3_out_rec[0], t3_out_nieve[0], 8, 10));

	// odd sized matrix
	double t4_out_rec[7][9];
	double t4_out_nieve[7][9];
	ra::cache::matrix_transpose(test_4[0], 9, 7, t4_out_rec[0]);
	ra::cache::naive_matrix_transpose(test_4[0], 9, 7, t4_out_nieve[0]);
	REQUIRE(comp_matrix(t4_out_rec[0], t4_out_nieve[0], 7, 9));
}
