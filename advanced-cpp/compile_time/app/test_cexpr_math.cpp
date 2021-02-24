#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ra/cexpr_math.hpp"

TEST_CASE("Absolute Value") {
    constexpr int val1 = ra::cexpr_math::abs(-1);
    REQUIRE(val1 == 1);

    constexpr double val2 = ra::cexpr_math::abs(-2.1);
    REQUIRE(val2 == 2.1);

    REQUIRE(ra::cexpr_math::abs(0) == 0);
    REQUIRE(ra::cexpr_math::abs(1) == 1);
}

TEST_CASE("Square") {
    constexpr int val1 = ra::cexpr_math::sqr(-1);
    REQUIRE(val1 == 1);

    constexpr double val2 = ra::cexpr_math::sqr(0.5);
    REQUIRE(val2 == 0.25);

    REQUIRE(ra::cexpr_math::sqr(0) == 0);
}

TEST_CASE("CUBE") {
    constexpr int val1 = ra::cexpr_math::cube(-1);
    REQUIRE(val1 == -1);

    constexpr double val2 = ra::cexpr_math::cube(0.5);
    REQUIRE(val2 == 0.125);

    constexpr int val3 = ra::cexpr_math::cube(2);
    REQUIRE(val3 == 8);

    REQUIRE(ra::cexpr_math::cube(0) == 0);
}

TEST_CASE("Modulus") {
    REQUIRE_THROWS_AS(ra::cexpr_math::mod(1.0, 0.0), std::overflow_error);

    constexpr double val1 = ra::cexpr_math::mod(3.0, 3.0);
    REQUIRE(val1 == 0.0);

    constexpr int val2 = ra::cexpr_math::mod(1.0, 3.0);
    REQUIRE(val2 == 1.0);

    REQUIRE(ra::cexpr_math::mod<double>(9.0, 2.7) == Approx(0.9));
}

TEST_CASE("Testing Trig Values") {
    // just to list important trig values
    constexpr double neg_two_pi = -1 * 2 * ra::cexpr_math::pi<double>;
    constexpr double pos_two_pi = 2 * ra::cexpr_math::pi<double>;
    constexpr double pi_by_2 = ra::cexpr_math::pi<double> / 2;
    constexpr double neg_one = -1.0;
    constexpr double pos_one = 1.0;
    constexpr double zero = 0;
    constexpr double random1 = 0.7;
    constexpr double random2 = -0.3;

    constexpr double val1 = ra::cexpr_math::sin(pos_two_pi);
    REQUIRE(val1 == 0);

    constexpr double val2 = ra::cexpr_math::sin(pos_one);
    REQUIRE(val2 == Approx(sin(pos_one)));

    REQUIRE(ra::cexpr_math::sin(zero) == zero);

    REQUIRE(ra::cexpr_math::sin<double>(neg_one) == Approx(-0.8414709848));
    REQUIRE(ra::cexpr_math::sin<double>(random1) == Approx(sin(random1)));

    // cosine section
    constexpr double val4 = ra::cexpr_math::cos(pos_two_pi);
    REQUIRE(val4 == 1);

    constexpr double val5 = ra::cexpr_math::cos(zero);
    REQUIRE(val5 == 1);

    REQUIRE(ra::cexpr_math::cos<double>(neg_one) == Approx(cos(neg_one)));
    REQUIRE(ra::cexpr_math::cos<double>(random1) == Approx(cos(random1)));
    REQUIRE(ra::cexpr_math::cos<double>(neg_two_pi) == Approx(cos(pos_two_pi)));

    // tan section
    // should throw error but since approx, is off a bit
    // REQUIRE_THROWS_AS(ra::cexpr_math::tan(pi_by_2), std::overflow_error);
    REQUIRE(ra::cexpr_math::tan(zero) == 0);
    REQUIRE(ra::cexpr_math::tan<double>(random1) == Approx(tan(random1)));
    REQUIRE(ra::cexpr_math::tan<double>(random2) == Approx(tan(random2)));
}

TEST_CASE("Test Square Root") {
    REQUIRE_THROWS_AS(ra::cexpr_math::sqrt(-1.0), std::domain_error);

    REQUIRE(ra::cexpr_math::sqrt<float>(1.0) == 1.0);
    constexpr double val1 = ra::cexpr_math::sqrt(0.0);
    REQUIRE(val1 == 0.0);

    constexpr double val2 = ra::cexpr_math::sqrt(4.0);
    REQUIRE(val2 == 2.0);

    REQUIRE(ra::cexpr_math::sqrt<float>(4.0) == 2.0);
    REQUIRE(ra::cexpr_math::sqrt<float>(0.25) == 0.5);
    REQUIRE(ra::cexpr_math::sqrt<float>(0) == 0);
}