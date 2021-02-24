#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ra/random.hpp"
#include <random>
#include <iostream>

TEST_CASE("Making a LCG") {
	ra::random::linear_congruential_generator gen (2, 3, 8);
	
	std::ostringstream stream;
	stream << gen;
	
	REQUIRE(stream.str() == "2 3 8 1");
	stream.str("");

	ra::random::linear_congruential_generator gen2 (1, 0, 2, 0);
	stream << gen2;
	REQUIRE(stream.str() == "1 0 2 1");	
}

TEST_CASE("Generating Items") {
	std::linear_congruential_engine<unsigned long long int, 2, 3, 8> lib_gen(5);
	ra::random::linear_congruential_generator gen (2, 3, 8, 5);
	REQUIRE(lib_gen() == gen());

	lib_gen.discard(5);
	gen.discard(5);
	REQUIRE(lib_gen() == gen());

}

TEST_CASE("Getting Values") {
	const ra::random::linear_congruential_generator gen(2, 3, 5, 4);
	const ra::random::linear_congruential_generator gen2(2, 0, 5, 4);
	
	REQUIRE(gen.multiplier() == 2);
	REQUIRE(gen.increment() == 3);
	REQUIRE(gen.modulus() == 5);
	REQUIRE(gen.default_seed() == 1);

	REQUIRE(gen.min() == 0);
	REQUIRE(gen.max() == 4);

	REQUIRE(gen2.min() == 1);


	ra::random::linear_congruential_generator gen3(2, 0, 6, 0);
	REQUIRE(gen3() == 2);

}

TEST_CASE("Standard Out") {
	const ra::random::linear_congruential_generator gen2(2, 0, 5, 4);

	std::ostringstream stream;
	stream << gen2;
	
	REQUIRE(stream.str() == "2 0 5 4");	
}

TEST_CASE("Comparisons") {
	const ra::random::linear_congruential_generator gen (2, 3, 5, 4);
	ra::random::linear_congruential_generator gen2(2, 3, 5, 4);
	const ra::random::linear_congruential_generator gen3(2, 0, 5, 4);
	const ra::random::linear_congruential_generator gen4(2, 1, 5, 4);

	gen2();
	REQUIRE(!(gen == gen2));
	REQUIRE(gen != gen2);
	REQUIRE(gen3 != gen4);
	REQUIRE(!(gen3 == gen4));
}


