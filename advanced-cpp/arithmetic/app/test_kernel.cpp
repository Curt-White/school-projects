#define CATCH_CONFIG_MAIN

#include "ra/kernel.hpp"
#include "catch2/catch.hpp"

const double numbers[] = {
	0.234445454757488547848494324,
	2.893248342579238328234803453,
	4.723479237892579234001104567,
	-2.67383848484939202293842457,
	-5.72382923740239423048507543,
	3.792392347923404081348017498
};

TEST_CASE("2D Orientation") {
	using O = ra::geometry::Kernel<double>::Orientation;
	ra::geometry::Kernel<double> kern;

	ra::geometry::Kernel<double>::Point a(0.0, 4.0), b(2.0, 2.0), c(2.0, 0.0);
	auto res = kern.orientation(a, b, c);
	REQUIRE(res == ra::geometry::Kernel<double>::Orientation::right_turn);


	ra::geometry::Kernel<double>::Point a2(0.0, 4.0), b2(2.0, 2.0), c2(4.0, 4.0);
	auto res2 = kern.orientation(a2, b2, c2);
	REQUIRE(res2 == ra::geometry::Kernel<double>::Orientation::left_turn);

	ra::geometry::Kernel<double>::Point a3(0.0, 0.0), b3(2.0, 2.0), c3(1.0, 1.0);
	auto res3 = kern.orientation(a3, b3, c3);
	REQUIRE(res3 == ra::geometry::Kernel<double>::Orientation::collinear);

	ra::geometry::Kernel<double>::Point a4(0.0, 0.0), b4(1.0, 0.0), c4(1.0, 1.0), d4(0.0, 1.0);
	REQUIRE(kern.orientation(a4, b4, c4) == O::left_turn);
	REQUIRE(kern.orientation(b4, c4, d4) == O::left_turn);
	REQUIRE(kern.orientation(c4, d4, a4) == O::left_turn);
	REQUIRE(kern.orientation(d4, a4, b4) == O::left_turn);

	ra::geometry::Kernel<double>::Statistics s;
	kern.get_statistics(s);
	REQUIRE(s.orientation_total_count == 7);
	REQUIRE(s.orientation_exact_count == 0);

	kern.clear_statistics();
	kern.get_statistics(s);
	REQUIRE(s.orientation_total_count == 0);
	REQUIRE(s.orientation_exact_count == 0);

	ra::geometry::Kernel<double>::Point a5(0.0, 0.0), b5(0.000000000000001, 0.0), c5(0.00000000000005, 0.000000000000000000111);
	REQUIRE(kern.orientation(a5, b5, c5) == O::left_turn);
}

TEST_CASE("Side Of Circle") {
	ra::geometry::Kernel<float> kern;
	ra::geometry::Kernel<float>::Point a(0.0, 0.0), b(2.0, 0.0), c(0.0, 2.0), d(1.0, 1.0);
	auto res = kern.side_of_oriented_circle(a, b, c, d);
	REQUIRE(res == ra::geometry::Kernel<float>::Oriented_side::on_positive_side);

	ra::geometry::Kernel<float>::Point e(2.0, 2.0), f(3.0, 3.0);
	auto res2 = kern.side_of_oriented_circle(a, b, c, e);
	REQUIRE(res2 == ra::geometry::Kernel<float>::Oriented_side::on_boundary);

	auto res3 = kern.side_of_oriented_circle(a, b, c, f);
	REQUIRE(res3 == ra::geometry::Kernel<float>::Oriented_side::on_negative_side);

	ra::geometry::Kernel<float>::Statistics s;
	kern.get_statistics(s);
	REQUIRE(s.side_of_oriented_circle_total_count == 3);
	REQUIRE(s.side_of_oriented_circle_exact_count == 0);
}

TEST_CASE("Preferred Direction") {
	ra::geometry::Kernel<float> kern;
	ra::geometry::Kernel<float>::Point a(0.0, 0.0), b(2.0, 2.0), c(2.0, 0.0), d(0.0, 2.0);
	ra::geometry::Kernel<float>::Vector u(2, 1), v(1, 0), w(-1, 2);
	
	REQUIRE(kern.preferred_direction(a, b, c, d, u) == 1);
	REQUIRE(kern.preferred_direction(a, b, c, d, v) == 0);
	REQUIRE(kern.preferred_direction(a, b, c, d, w) == -1);

	ra::geometry::Kernel<float>::Statistics s;
	kern.get_statistics(s);
	REQUIRE(s.preferred_direction_total_count == 3);
	REQUIRE(s.preferred_direction_exact_count == 0);
}

TEST_CASE("Strict Convex") {
	ra::geometry::Kernel<float> kern;
	ra::geometry::Kernel<float>::Point a(0.0, 0.0), b(1.0, 0.0), c(1.0, 1.0), d(0.0, 1.0);

	REQUIRE(kern.is_strictly_convex_quad(a, b, c, d));

	// convex but not strictly
	ra::geometry::Kernel<float>::Point a1(0.0, 0.0), b1(0.0, 1.0), c1(0.0, 2.0), d1(-1.0, 1.0);
	REQUIRE(!kern.is_strictly_convex_quad(a1, b1, c1, d1));

	// slightly convex
	ra::geometry::Kernel<float>::Point a2(0.0, 0.0), b2(0.00001, 1.0), c2(0.0, 2.0), d2(-1.0, 1.0);
	REQUIRE(kern.is_strictly_convex_quad(a2, b2, c2, d2));

	// slightly not convex
	ra::geometry::Kernel<float>::Point a3(0.0, 0.0), b3(-0.00001, 1.0), c3(0.0, 2.0), d3(-1.0, 1.0);
	REQUIRE(!kern.is_strictly_convex_quad(a3, b3, c3, d3));
}

TEST_CASE("Local Dealuany") {
	ra::geometry::Kernel<float> kern;
	ra::geometry::Kernel<float>::Point a(0.0, 0.0), b(1.0, 1.0), d(-1.0, 1.0), c(0.0, 2.0);

	REQUIRE(kern.is_locally_delaunay_edge(a, b, c, d));
	
	ra::geometry::Kernel<float>::Point a2(0.0, 0.0), b2(1.0, 1.0), d2(-1.0, 1.0), c2(0.0, 1.9);
	REQUIRE(kern.is_locally_delaunay_edge(a2, b2, c2, d2));

	ra::geometry::Kernel<float>::Point a3(0.0, 0.0), b3(1.0, 1.0), d3(-1.0, 1.0), c3(0.0, 2.000001);
	REQUIRE(!kern.is_locally_delaunay_edge(a3, b3, c3, d3));
}

TEST_CASE("DP Delauny") {
	ra::geometry::Kernel<double> kern;
	ra::geometry::Kernel<double>::Vector u(2, 1), v(1, 0), w(-1, 1), y(0, 1);

	ra::geometry::Kernel<double>::Point a2(0.0, 0.0), b2(1.0, 1.0), d2(-1.0, 1.0), c2(0.0, 1.9);
	REQUIRE(kern.is_locally_pd_delaunay_edge(a2, b2, c2, d2, u, v));

	ra::geometry::Kernel<double>::Point a3(0.0, 0.0), b3(1.0, 1.0), d3(-1.0, 1.0), c3(0.0, 2.000001);
	REQUIRE(!kern.is_locally_pd_delaunay_edge(a3, b3, c3, d3, u, v));

	ra::geometry::Kernel<double>::Point a(0.0, 0.0), b(1.0, 1.0), d(-1.0, 1.0), c(0.0, 2.0);
	REQUIRE(!kern.is_locally_pd_delaunay_edge(a, b, c, d, u, v));
	REQUIRE(kern.is_locally_pd_delaunay_edge(a, b, c, d, y, w));
	REQUIRE(kern.is_locally_pd_delaunay_edge(a, b, c, d, w, y));
	REQUIRE(!kern.is_locally_pd_delaunay_edge(a, b, c, d, w, v));
}