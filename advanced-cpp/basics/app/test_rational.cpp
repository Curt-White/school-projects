#define CATCH_CONFIG_MAIN

#include <limits>
#include <iostream>
#include <catch2/catch.hpp>
#include "ra/rational.hpp"

template<class T>
bool requals(const ra::math::rational<T> &frac, T n, T d) {
    if (frac.numerator() == n && frac.denominator() == d) {
        return true;
    }

    std::cout << frac << std::endl;
    return false;
}

TEST_CASE("Constructing a Rational Number") {
    SECTION("Test Default Constructor Arguments") {
        ra::math::rational<int> rnum;
        REQUIRE(rnum.numerator() == 0);
        REQUIRE(rnum.denominator() == 1);
    }

    SECTION("2 Arg Constructor") {
        ra::math::rational<int> rnum(2, 3);
        REQUIRE(rnum.numerator() == 2);
        REQUIRE(rnum.denominator() == 3);
    }

    SECTION("Default Values") {
        ra::math::rational<int> rnum(2);
        REQUIRE(rnum.numerator() == 2);
        REQUIRE(rnum.denominator() == 1);
    }

    SECTION("Init 0 Denom") {
        ra::math::rational<int> rnum(2, 0);
        REQUIRE(rnum.numerator() == std::numeric_limits<int>::max());
        REQUIRE(rnum.denominator() == 1);
    }
}

TEST_CASE("+= Operator") { 
    SECTION("Basic Adding") {
	const ra::math::rational<int> rhs(1, 2);    
    	ra::math::rational<int> rnum(30, 4);
	
	rnum += rhs;
	REQUIRE(rnum.numerator() == 8);
	REQUIRE(rnum.denominator() == 1);
    }

    SECTION("Adding a Negative to 0") {
    	ra::math::rational<long int> lhs(-3, 4);
	ra::math::rational<long int> rhs(3, 4);

	rhs += lhs;
    	REQUIRE(rhs.numerator() == 0);
	REQUIRE(rhs.denominator() == 1);
	
	rhs += lhs;
    	REQUIRE(rhs.numerator() == -3);
	REQUIRE(rhs.denominator() == 4);
    }

    SECTION("Adding Overflow") {
    	ra::math::rational<int> lhs(1, 0);
	ra::math::rational<int> rhs(1, 1);

	// rhs += lhs;
	// std::cout << rhs << std::endl;
   	// REQUIRE(rhs.numerator() == std::numeric_limits<int>::min());	
   	// REQUIRE(rhs.denominator() == 1); 
    }
}

TEST_CASE("-= Operation") {
    SECTION("Basic Subtraction") {
    	ra::math::rational<int> lhs(15, 60);
	const ra::math::rational<int> rhs(30, 120);

	lhs -= rhs;
        REQUIRE(lhs.numerator() == 0);
	REQUIRE(lhs.denominator() == 1);
    }

    SECTION("Subtractin A Negative") {
    	ra::math::rational<int> lhs(0, 1);
	ra::math::rational<int> rhs(-15, 45);
    	
	lhs -= rhs;
	REQUIRE(lhs.numerator() == 1);
	REQUIRE(lhs.denominator() == 3);
    }
}

TEST_CASE("*= Operation") {
    SECTION("Basic Multiplication") {
    	ra::math::rational<int> lhs(2, 10);
	ra::math::rational<int> rhs(3, 10);

	lhs *= rhs;
	REQUIRE(lhs.numerator() == 3);
	REQUIRE(lhs.denominator() == 50);

	const ra::math::rational<int> rhs2(0, 10);
	REQUIRE(rhs2.numerator() == 0);
	REQUIRE(rhs2.denominator() == 1);

	lhs *= rhs2;
	REQUIRE(lhs.numerator() == 0);
	REQUIRE(lhs.denominator() == 1);

	lhs *= rhs;
	REQUIRE(lhs.numerator() == 0);
	REQUIRE(lhs.denominator() == 1);
	REQUIRE(requals(lhs, 0, 1));

	ra::math::rational<int> lhs2(4, -3);
	lhs2 *= rhs;
	REQUIRE(requals(lhs2, -2, 5));
    }
}

TEST_CASE("/= Operation") {
    ra::math::rational<int> lhs(40, 100);
    const ra::math::rational<int> rhs(100, 40);

    lhs /= rhs;
    REQUIRE(requals(lhs, 4, 25));
    REQUIRE(lhs.is_integer() == false);

    const ra::math::rational<int> rhs2(0, 12);
    lhs /= rhs2;
    REQUIRE(requals(lhs, std::numeric_limits<int>::max(), 1));
}

TEST_CASE("Not Operator") {
	const ra::math::rational<int> f1(1, 3);
	REQUIRE(!f1 == false);

	const ra::math::rational<int> f2(0, 3);
	REQUIRE(!f2 == true);
}

TEST_CASE("Not Equals") {
	const ra::math::rational<int> lhs(1, 5);
	const ra::math::rational<int> rhs(20, 100);
	REQUIRE((lhs != rhs) == false);

	const ra::math::rational<int> rhs2(0, 100);
	REQUIRE((lhs != rhs2) == true);
}

TEST_CASE("Equals Operator") {
	const ra::math::rational<int> lhs(1, 5);
	const ra::math::rational<int> rhs(20, 100);
	REQUIRE(lhs == rhs);

	const ra::math::rational<int> rhs2(0, 100);
	REQUIRE((lhs == rhs2) == false);
}

TEST_CASE("Comparisson Ops >, <, >=, <=") {
	const ra::math::rational<int> f1(1, 5);
	const ra::math::rational<int> f2(2, 5);
	const ra::math::rational<int> f3(0, 45);
	const ra::math::rational<int> f4(-5, 5);

	REQUIRE(f4.is_integer() == true);
	
	REQUIRE(f1 < f2);
	REQUIRE(f1 <= f2);
	REQUIRE(f2 > f1);
	REQUIRE(f2 >= f1);
	REQUIRE(f4 < f3);
	REQUIRE(f1 <= f1);
	REQUIRE(f2 >= f2);
}

TEST_CASE("Prefix/Posfix Inc/Dec") {
	ra::math::rational<int> f1(3, 10);
	ra::math::rational<int> f2(0, 43);

	REQUIRE(requals(++f1, 13, 10));
	REQUIRE(requals(++f2, 1, 1));

	REQUIRE(requals(f1++, 13, 10));
	REQUIRE(requals(f1, 23, 10));

	REQUIRE(requals(f2++, 1, 1));
	REQUIRE(requals(f2, 2, 1));

	REQUIRE(requals(--f1, 13, 10));
	REQUIRE(requals(--f2, 1, 1));


	REQUIRE(requals(--f1, 3, 10));
	REQUIRE(requals(--f1, -7, 10));

	REQUIRE(requals(f2--, 1, 1));
	REQUIRE(requals(f2--, 0, 1));
	REQUIRE(requals(f2, -1, 1));
}

TEST_CASE("Complement and Unary Positive Ops") {
	const ra::math::rational<int> f1(0, 1);
	const ra::math::rational<int> f2(4, 51);

	REQUIRE(requals(-f1, 0, 1));
	REQUIRE(requals(-f2, -4, 51));

	REQUIRE(requals(-(-f2), 4, 51));

	REQUIRE(requals(+f2, 4, 51));
	REQUIRE(requals(+f1, 0, 1));
}

TEST_CASE("Binary Add and Subtract") {
	const ra::math::rational<int> f1(0, 1);
	const ra::math::rational<int> f2(-13, 2);
	const ra::math::rational<int> f3(13, 5);

	REQUIRE(f1 + f2 == f2);
	REQUIRE(requals(f3 + f2 + f1, -39, 10));
	REQUIRE(requals(f2 + f3, -39, 10));

	REQUIRE(requals(f3 - f2, 91, 10));
	REQUIRE(requals(f2 - f3, -91, 10));
}

TEST_CASE("Binary Multiple and Division") {
	const ra::math::rational<int> f1(1, 3);
	const ra::math::rational<int> f2(-2, 5);
	const ra::math::rational<int> f3(0, 4);
	const ra::math::rational<int> f4(4, 15);

	REQUIRE(requals(f1 * f4, 4, 45));
	REQUIRE(requals(f2 * f1, -2, 15));
	REQUIRE(requals(f1 * f3, 0, 1));
	REQUIRE(requals(f1 * f1, 1, 9));

	REQUIRE(requals(f1 / f4, 5, 4));
	REQUIRE(requals(f2 / f1, -6, 5));
	REQUIRE(requals(f1 / f3, std::numeric_limits<int>::max(), 1));
	REQUIRE(requals(f1 / f1, 1, 1));
}

TEST_CASE("Stream Operators") {
	const ra::math::rational<int> f1(0, 3);
	const ra::math::rational<int> f2(-1, 5);
	const ra::math::rational<int> f3(4, 3);
	
	std::ostringstream fstr;
	fstr << f1;
	REQUIRE(fstr.str() == "0/1");
	fstr.str("");

	fstr << f2;
	REQUIRE(fstr.str() == "-1/5");
	fstr.str("");

	fstr << f3;
	REQUIRE(fstr.str() == "4/3");
	fstr.str("");
	
	ra::math::rational<int> f4(0, 3);
	std::istringstream infstr;

	infstr.str("-2/4");
	infstr >> f4;
	REQUIRE(requals(f4, -1, 2));
	REQUIRE((infstr.rdstate() & std::ifstream::failbit) == 0);
	infstr.str("");
	infstr.clear();

	infstr.str("1/3");
	infstr >> f4;
	REQUIRE(requals(f4, 1, 3));
	REQUIRE((infstr.rdstate() & std::ifstream::failbit) == 0);
	infstr.str("");
	infstr.clear();

	infstr.str("0/4");
	infstr >> f4;
	REQUIRE(requals(f4, 0, 1));
	REQUIRE((infstr.rdstate() & std::ifstream::failbit) == 0);
	infstr.str("");
	infstr.clear();

	infstr.str("1/-4");
	infstr >> f4;
	REQUIRE(requals(f4, -1, 4));
	REQUIRE((infstr.rdstate() & std::ifstream::failbit) == 0);
	infstr.str("");
	infstr.clear();
	
	infstr.str("1.7/2");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();
	
	infstr.str("1p2");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str(" 1/2");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str("1/2 ");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();
	
	infstr.str("1 /2");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str("1 2");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str("1/");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str("1/ 4");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();

	infstr.str("1/i");
	infstr >> f4;
	REQUIRE(infstr.fail());
	infstr.str("");
	infstr.clear();
}

TEST_CASE("Other Funcs") {
	const ra::math::rational<int> f1(1, 3);
	const ra::math::rational<int> f2(-1, 3);
	const ra::math::rational<int> f3(3, 3);
	const ra::math::rational<int> f4(0, 4);

	REQUIRE(!f1.is_integer());
	REQUIRE(!f2.is_integer());
	REQUIRE(f3.is_integer());
	REQUIRE(f4.is_integer());

	const ra::math::rational<int> f5(46, 20);
	const ra::math::rational<int> f6(-63, 10);
	REQUIRE(f1.truncate() == 0);
	REQUIRE(f2.truncate() == 0);
	REQUIRE(f3.truncate() == 1);
	REQUIRE(f4.truncate() == 0);
	REQUIRE(f5.truncate() == 2);
	REQUIRE(f6.truncate() == -6);
}
