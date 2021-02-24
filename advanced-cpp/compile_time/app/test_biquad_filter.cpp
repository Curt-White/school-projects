#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "ra/biquad_filter.hpp"

#define norm_freq(fc, fz) (2.0 * (double) fc / (double) fz)

TEST_CASE("Making a biquad filter") {
	constexpr auto bqf = ra::biquad::biquad_filter_coefs (
		2, 2, 2, 2, 2, 4
	);

	REQUIRE(bqf.a0 == 1);
	REQUIRE(bqf.b0 == 1);
	REQUIRE(bqf.b2 == 2);

	constexpr auto bqf2 = ra::biquad::biquad_filter_coefs(bqf);
	REQUIRE(bqf.a0 == 1);
	REQUIRE(bqf.b0 == 1);
	REQUIRE(bqf.b2 == 2);
}

TEST_CASE("Test low pass filters") {
	constexpr auto lp = ra::biquad::lowpass(norm_freq(10000, 44100), 0.7071);
	REQUIRE(lp.a0 == Approx(0.2513790015));
	REQUIRE(lp.a1 == Approx(0.5027580030));
	REQUIRE(lp.a2 == Approx(0.2513790015));
	REQUIRE(lp.b0 == Approx(1.0));
	REQUIRE(lp.b1 == Approx(-0.1712407144));
	REQUIRE(lp.b2 == Approx(0.17675672047));
}

TEST_CASE("Test high pass filters") {
	constexpr auto lp = ra::biquad::highpass(norm_freq(10000.0, 44100.0), 0.7071);
	REQUIRE(lp.a0 == Approx(0.33699935872));
	REQUIRE(lp.a1 == Approx(-0.673998717440));
	REQUIRE(lp.a2 == Approx(0.33699935872));
	REQUIRE(lp.b0 == Approx(1.0));
	REQUIRE(lp.b1 == Approx(-0.1712407144));
	REQUIRE(lp.b2 == Approx(0.17675672047));
}

TEST_CASE("Test band pass filters") {
	constexpr auto lp = ra::biquad::bandpass(norm_freq(10000, 44100), 0.7071);
	REQUIRE(lp.a0 == Approx(0.4116216397667));
	REQUIRE(lp.a1 == Approx(0));
	REQUIRE(lp.a2 == Approx(-0.41162163976));
	REQUIRE(lp.b0 == Approx(1.0));
	REQUIRE(lp.b1 == Approx(-0.171240714413));
	REQUIRE(lp.b2 == Approx(0.17675672046));
}

TEST_CASE("Test low shelf boost filters") {
	constexpr auto lp = ra::biquad::lowshelf_boost(norm_freq(10000, 44100), 1.12201845);
	REQUIRE(lp.a0 == Approx(1.055062976185));
	REQUIRE(lp.a1 == Approx(-0.109895393805));
	REQUIRE(lp.a2 == Approx(0.183044386090));
	REQUIRE(lp.b0 == Approx(1.0));
	REQUIRE(lp.b1 == Approx(-0.17124139038));
	REQUIRE(lp.b2 == Approx(0.1767613656973));
}

TEST_CASE("Test low shelf cut filters") {
	constexpr auto lp = ra::biquad::lowshelf_cut(norm_freq(10000, 44100), 1.12201845);
	REQUIRE(lp.a0 == Approx(0.947810720849));
	REQUIRE(lp.a1 == Approx(-0.1623044256594));
	REQUIRE(lp.a2 == Approx(0.167536317439));
	REQUIRE(lp.b0 == Approx(1.0));
	REQUIRE(lp.b1 == Approx(-0.104160032420));
	REQUIRE(lp.b2 == Approx(0.1734914315279));
}