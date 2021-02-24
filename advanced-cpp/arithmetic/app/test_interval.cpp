#define CATCH_CONFIG_MAIN

#include "ra/interval.hpp"
#include "catch2/catch.hpp"
#include "boost/numeric/interval.hpp"

#include <iostream>
#include <limits>

#define IRRAT1 6.2831853071795800
#define IRRAT2 2.7182818284590452353602874713527

TEST_CASE("Test Creation") {
	ra::math::interval<float> i1;
	REQUIRE(i1.lower() == 0);
	REQUIRE(i1.upper() == 0);

	ra::math::interval<double> i2(1.4, 3.5);
	REQUIRE(i2.lower() == 1.4);
	REQUIRE(i2.upper() == 3.5);
	
	ra::math::interval<double>::statistics stats;
	ra::math::interval<double>::get_statistics(stats);
	REQUIRE(stats.indeterminate_result_count == 0);
	REQUIRE(stats.arithmetic_op_count == 0);
}

TEST_CASE("Adding") {
	std::fesetround(FE_TOWARDZERO);

	double d = 3.14159265358979;
	ra::math::interval<double> i1(d);
	ra::math::interval<double> i2(d);
	ra::math::interval<double>::statistics stats;

	// Generic binary adding example
	auto i3 = i1 + i2;
	REQUIRE(i3.lower() == 6.2831853071795800);
	REQUIRE(i3.upper() == 6.2831853071795801);

	ra::math::interval<double>::get_statistics(stats);
	REQUIRE(stats.indeterminate_result_count == 0);
	REQUIRE(stats.arithmetic_op_count == 1);

	// Generic compund add example
	i1 += i2;
	REQUIRE(i1.lower() == 6.2831853071795800);
	REQUIRE(i1.upper() == 6.2831853071795801);

	// Checking rounding state was restored
	REQUIRE(std::fegetround() == FE_TOWARDZERO);
	ra::math::interval<double>::get_statistics(stats);
	REQUIRE(stats.arithmetic_op_count == 2);

	// Compare with the Boost interval library 
	boost::numeric::interval<double> ib1(IRRAT1), ib2(IRRAT2);
	auto ib3 = ib1 + ib2;

	ra::math::interval<double> i4(IRRAT1), i5(IRRAT2);
	auto i6 = i4 + i5;

	REQUIRE(ib3.upper() == i6.upper());
	REQUIRE(ib3.lower() == i6.lower());
	REQUIRE(std::fegetround() == FE_TOWARDZERO);

	// Compare with the Boost interval library
	boost::numeric::interval<double> ib4(IRRAT1), ib5(IRRAT2);
	ib4 += ib5;

	ra::math::interval<double> i7(IRRAT1), i8(IRRAT2);
	i7 += i8;

	REQUIRE(ib4.upper() == i7.upper());
	REQUIRE(ib4.lower() == i7.lower());

	ib4 += ib4;
	i7 += i7;
	REQUIRE(ib4.upper() == i7.upper());
	REQUIRE(ib4.lower() == i7.lower());

	i7 = i7 + i7;
	ib4 = ib4 + ib4;
	REQUIRE(ib4.upper() == i7.upper());
	REQUIRE(ib4.lower() == i7.lower());
}

TEST_CASE("Subtraction") {
	ra::math::interval<double>::statistics stats;

	SECTION("Binary Subtract") {
		std::fesetround(FE_TONEAREST);
		boost::numeric::interval<double> ib1(IRRAT1), ib2(IRRAT2);
		auto ib3 = ib1 - ib2;

		ra::math::interval<double> i4(IRRAT1), i5(IRRAT2);
		auto i6 = i4 - i5;

		REQUIRE(ib3.upper() == i6.upper());
		REQUIRE(ib3.lower() == i6.lower());
		REQUIRE(std::fegetround() == FE_TONEAREST);
		ra::math::interval<double>::get_statistics(stats);
		REQUIRE(stats.arithmetic_op_count == 7);
	}

	SECTION("Compound Subtract") {
		boost::numeric::interval<double> ib4(IRRAT1), ib5(IRRAT2);
		ib4 -= ib5;

		ra::math::interval<double> i7(IRRAT1), i8(IRRAT2);
		i7 -= i8;

		REQUIRE(ib4.upper() == i7.upper());
		REQUIRE(ib4.lower() == i7.lower());
	}

	SECTION("Alias Testing") {
		// TODO: test the stats counters
		boost::numeric::interval<long double> ib4(IRRAT1), ib5(IRRAT2);
		ib4 -= ib5;

		ra::math::interval<long double> i7(IRRAT1), i8(IRRAT2);
		i7 -= i8;

		ib4 += ib4;
		i7 += i7;
		REQUIRE(ib4.upper() == i7.upper());
		REQUIRE(ib4.lower() == i7.lower());

		i7 = i7 + i7;
		ib4 = ib4 + ib4;
		REQUIRE(ib4.upper() == i7.upper());
		REQUIRE(ib4.lower() == i7.lower());
	}
}

TEST_CASE("Multiplication") {
	SECTION("Binary Multiplication") {
		boost::numeric::interval<double> ib1(IRRAT1), ib2(IRRAT2);
		auto ib3 = ib1 * ib2;

		ra::math::interval<double> i4(IRRAT1), i5(IRRAT2);
		auto i6 = i4 * i5;

		REQUIRE(ib3.upper() == i6.upper());
		REQUIRE(ib3.lower() == i6.lower());

		boost::numeric::interval<double> ib4(IRRAT1), ib5(-IRRAT2);
		auto ib6 = ib4 * ib5;
		auto ib7 = ib5 * ib4;

		ra::math::interval<double> i8(IRRAT1), i9(-IRRAT2);
		auto i7 = i9 * i8;
		auto i10 = i8 * i9;

		REQUIRE(ib6.upper() == i7.upper());
		REQUIRE(ib6.lower() == i7.lower());

		REQUIRE(ib7.upper() == i10.upper());
		REQUIRE(ib7.lower() == i10.lower());

		boost::numeric::interval<double> ib20(-IRRAT1, -IRRAT2), ib21(-IRRAT1, IRRAT2);
		ra::math::interval<double> i20(-IRRAT1, -IRRAT2), i21(-IRRAT1, IRRAT2);
		// lhs < 0 rhs < 0
		auto i22 = i20 * i20;
		auto ib22 = ib20 * ib20;
		REQUIRE(ib22.upper() == i22.upper());
		REQUIRE(ib22.lower() == i22.lower());

		// lhs ub < 0 & rhslb < 0 rhsub > 0
		auto i23 = i20 * i21;
		auto ib23 = ib20 * ib21;
		REQUIRE(ib23.upper() == i23.upper());
		REQUIRE(ib23.lower() == i23.lower());

		i21 *= i21;
		ib21 *= ib21;
		REQUIRE(ib21.upper() == i21.upper());
		REQUIRE(ib21.lower() == i21.lower());

		ib1 *= ib21;
		i4 *= i21;
		REQUIRE(ib1.upper() == i4.upper());
		REQUIRE(ib1.lower() == i4.lower());

		auto ib24 = ib21 * ib20;
		auto i24 = i21 * i20;
		REQUIRE(ib24.upper() == i24.upper());
		REQUIRE(ib24.lower() == i24.lower());

		auto ib25 = ib21 * ib2;
		auto i25 = i21 * i5;
		REQUIRE(ib25.upper() == i25.upper());
		REQUIRE(ib25.lower() == i25.lower());
	}
}

TEST_CASE("Is Singleton") {
	const ra::math::interval<float> i1(IRRAT1), i2(-IRRAT1, -IRRAT2);
	REQUIRE(i1.is_singleton());
	REQUIRE(!i2.is_singleton());
}

TEST_CASE("Sign") {
	const ra::math::interval<float> i1(IRRAT1), i2(-IRRAT1, -IRRAT2), i3(0), i4(-IRRAT1, IRRAT2);
	REQUIRE(i1.sign() == 1);
	REQUIRE(i2.sign() == -1);
	REQUIRE(i3.sign() == 0);
	REQUIRE_THROWS_AS(i4.sign(), ra::math::indeterminate_result);
	
	ra::math::interval<float>::statistics stats;
	ra::math::interval<float>::get_statistics(stats);
	REQUIRE(stats.indeterminate_result_count == 1);
}

TEST_CASE("Test Stats") {
	ra::math::interval<float> i1(IRRAT1), i2(-IRRAT1, -IRRAT2);
	i1 += i2;

	ra::math::interval<float>::statistics stats;
	ra::math::interval<float>::get_statistics(stats);
	REQUIRE(stats.indeterminate_result_count != 0);
	REQUIRE(stats.arithmetic_op_count != 0);

	ra::math::interval<float>::clear_statistics();
	ra::math::interval<float>::get_statistics(stats);
	REQUIRE(stats.indeterminate_result_count == 0);
	REQUIRE(stats.arithmetic_op_count == 0);
}

TEST_CASE("Test Comparisson Operator") {
	const ra::math::interval<float> i1(IRRAT1), i2(-IRRAT1, -IRRAT2), i3(0), i4(-IRRAT1, IRRAT2);
	REQUIRE(i2 < i1);
	REQUIRE((i3 < i2) == false);
	REQUIRE_THROWS_AS(i4 < i3, ra::math::indeterminate_result);
}