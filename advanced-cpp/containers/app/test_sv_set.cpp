#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ra/sv_set.hpp"

using ra::container::sv_set;

TEST_CASE("Default Construction") {
	sv_set<int> c1;

	REQUIRE(c1.capacity() == 0);
	REQUIRE(c1.size() == 0);
	REQUIRE(c1.begin() == c1.end());
}

TEST_CASE("Unique Iter Construction") {
	int items[] = { 1, 2, 3, 10 };
	sv_set<int> c1(sv_set<int>::ordered_and_unique_range{}, items, 4);
	
	auto iter = c1.begin();
	REQUIRE(iter[0] == 1);
	REQUIRE(iter[1] == 2);
	REQUIRE(iter[2] == 3);
	REQUIRE(iter[3] == 10);

	REQUIRE(c1.capacity() == 4);
	REQUIRE(c1.size() == 4);
}

TEST_CASE("Move Construction") {
	int items[] = { 1, 24 };
	sv_set<int> c1(sv_set<int>::ordered_and_unique_range{}, items, 2);

	sv_set<int> c2(std::move(c1));

	auto iter = c2.begin();
	REQUIRE(iter[0] == 1);
	REQUIRE(iter[1] == 24);
	REQUIRE(c2.capacity() == 2);
	REQUIRE(c2.size() == 2);

	REQUIRE(c1.capacity() == 0);
	REQUIRE(c1.size() == 0);
	REQUIRE(c1.begin() == c1.end());
}

TEST_CASE("Move Assignnment") {
	int items[] = { 1, 24 };
	sv_set<int> c1(sv_set<int>::ordered_and_unique_range{}, items, 2);

	sv_set<int> c2;
	c2 = std::move(c1);

	auto iter = c2.begin();
	REQUIRE(iter[0] == 1);
	REQUIRE(iter[1] == 24);
	REQUIRE(c2.capacity() == 2);
	REQUIRE(c2.size() == 2);

	REQUIRE(c1.capacity() == 0);
	REQUIRE(c1.size() == 0);
	REQUIRE(c1.begin() == c1.end());
}

TEST_CASE("Copy Construction") {
	int items[] = { 1, 24 };
	sv_set<int> c1(sv_set<int>::ordered_and_unique_range{}, items, 2);
	c1.reserve(10);

	sv_set<int> c2(c1);
	auto iter = c2.begin();
	REQUIRE(iter[0] == 1);
	REQUIRE(iter[1] == 24);

	auto iter2 = c1.begin();
	REQUIRE(iter2[0] == 1);
	REQUIRE(iter2[1] == 24);

	REQUIRE(c1.capacity() == c2.capacity());
	REQUIRE(c1.size() == c2.size());
}

TEST_CASE("Copy Assignnment") {
	double items[] = { 1.0, 24.0 };
	sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items, 2);

	sv_set<double> c2;
	c2 = c1;

	auto iter = c2.begin();
	REQUIRE(iter[0] == 1);
	REQUIRE(iter[1] == 24);

	auto iter2 = c1.begin();
	REQUIRE(iter2[0] == 1);
	REQUIRE(iter2[1] == 24);

	REQUIRE(c1.capacity() == c2.capacity());
	REQUIRE(c1.size() == c2.size());
}

TEST_CASE("Reserve") {
	SECTION("Test Empty") {
		sv_set<double> c1;
		REQUIRE(c1.capacity() == 0);
		REQUIRE(c1.size() == 0);

		c1.reserve(21);
		REQUIRE(c1.capacity() == 21);
		REQUIRE(c1.size() == 0);

		c1.reserve(20);
		REQUIRE(c1.capacity() == 21);
		REQUIRE(c1.size() == 0);
	}

	SECTION("Test Precontaining") {
		double items[] = { 1.0, 24.0 };
		sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items, 2);

		REQUIRE(c1.capacity() == 2);
		REQUIRE(c1.size() == 2);

		c1.reserve(21);
		REQUIRE(c1.capacity() == 21);
		REQUIRE(c1.size() == 2);
	}
}

TEST_CASE("Test Shrink to Fit") {
	SECTION("Test Empty") {
		sv_set<short> c1;
		c1.reserve(21);

		REQUIRE(c1.capacity() == 21);
		c1.shrink_to_fit();

		REQUIRE(c1.capacity() == 0);
		REQUIRE(c1.begin() == c1.end());
	}

	SECTION("Test Precontaining") {
		double items[] = { 1.0, 24.0 };
		sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items, 2);
		
		c1.reserve(21);
		REQUIRE(c1.capacity() == 21);
		
		c1.shrink_to_fit();
		REQUIRE(c1.capacity() == 2);

		c1.shrink_to_fit();
		REQUIRE(c1.capacity() == 2);
	}
}

TEST_CASE("Test Find") {
	int items[] = { 1, 2, 4, 5, 23, 34, 38, 234, 456 };
	sv_set<int> c1(sv_set<int>::ordered_and_unique_range{}, items, 9);

	REQUIRE(c1.find(5) == c1.begin() + 3);
	REQUIRE(c1.find(1) == c1.begin() + 0);
	REQUIRE(c1.find(23) == c1.begin() + 4);
	REQUIRE(c1.find(38) == c1.begin() + 6);
	REQUIRE(c1.find(456) == c1.end() - 1);

	long items2[] = { 1, 2, 4, 5, 23, 34, 38, 234, 456 };
	const sv_set<long> c2(sv_set<long>::ordered_and_unique_range{}, items2, 9);

	REQUIRE(c2.find(5) == c2.begin() + 3);
	REQUIRE(c2.find(1) == c2.begin() + 0);
	REQUIRE(c2.find(23) == c2.begin() + 4);
	REQUIRE(c2.find(38) == c2.begin() + 6);
	REQUIRE(c2.find(456) == c2.end() - 1);

	// find on an empty array
	sv_set<int> c3;
	c3.reserve(4);
	REQUIRE(c3.find(3) == c3.end());
}

TEST_CASE("Test Swap") {
	double items[] = { 1.0, 24.0 };
	sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items, 2);

	double items2[] = { 2.3, 4.5, 6.7 };
	sv_set<double> c2(sv_set<double>::ordered_and_unique_range{}, items2, 3);

	c1.swap(c2);
	REQUIRE(*c1.begin() == 2.3);
	REQUIRE(*(c1.end() - 1) == 6.7);
	REQUIRE(c1.capacity() == 3);

	REQUIRE(*c2.begin() == 1.0);
	REQUIRE(*(c2.end() - 1) == 24.0);
	REQUIRE(c2.capacity() == 2);
}

TEST_CASE("Test Erase") {
	double items1[] = { 2.3, 4.5, 6.7, 7.7, 9.5 };
	sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items1, 5);
	c1.reserve(10);

	REQUIRE(c1.size() == 5);
	REQUIRE(c1.capacity() == 10);

	auto rem1 = c1.erase(c1.begin() + 2);
	REQUIRE(*rem1 == 7.7);
	REQUIRE(c1.size() == 4);
	
	auto iter1 = c1.begin();
	REQUIRE(*(iter1 + 1) == 4.5);
	REQUIRE(*(iter1 + 2) == 7.7);
	REQUIRE(*(iter1 + 3) == 9.5);
	
	rem1 = c1.erase(c1.begin());
	REQUIRE(*rem1 == 4.5);
	REQUIRE(c1.size() == 3);
	iter1 = c1.begin();
	REQUIRE(*(iter1 + 0) == 4.5);
	REQUIRE(*(iter1 + 1) == 7.7);
	REQUIRE(*(iter1 + 2) == 9.5);

	rem1 = c1.erase(c1.end() - 1);
	REQUIRE(rem1 == c1.end());
	REQUIRE(*(iter1 + 0) == 4.5);
	REQUIRE(*(iter1 + 1) == 7.7);

	rem1 = c1.erase(c1.end() - 1);
	rem1 = c1.erase(c1.end() - 1);
	REQUIRE(c1.size() == 0);
	REQUIRE(c1.begin() == c1.end());
}

TEST_CASE("Test Clear") {
	double items1[] = { 2.3, 4.5, 6.7, 7.7, 9.5 };
	sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items1, 5);

	c1.clear();
	REQUIRE(c1.size() == 0);
}

TEST_CASE("Test Insert") {
	// just for use during manual testing
	auto tprint = [] (auto el) { 
		for (auto item : el) {
			std::cout << item << ", ";
		}
		std::cout << std::endl;
	};

	SECTION("Into Empty") {
		sv_set<int> c1;
		REQUIRE(c1.size() == 0);

		c1.insert(2);
		c1.insert(1);

		REQUIRE(*c1.begin() == 1);
		REQUIRE(*(c1.begin() + 1) == 2);
		REQUIRE(c1.size() == 2);
	}

	SECTION("Into Full") {
		double items1[] = { 2.3, 4.5, 6.7, 7.7, 9.5 };
		sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items1, 5);
		
		REQUIRE(c1.size() == 5);
		REQUIRE(c1.capacity() == 5);

		auto a1 = c1.insert(1.2);
		REQUIRE(a1.first == c1.begin());
		REQUIRE(c1.size() == 6);
		REQUIRE(a1.second == true);

		auto a2 = c1.insert(10.4);
		REQUIRE(a2.first == c1.end() - 1);
		REQUIRE(a2.second == true);

		REQUIRE(*c1.begin() == 1.2);
		REQUIRE(*(c1.end() - 1) == 10.4);
		REQUIRE(c1.size() == 7);
		REQUIRE(c1.capacity() == 7);

		auto a3 = c1.insert(10.4);
		REQUIRE(a2.first == a3.first);
		REQUIRE(a3.second == false);
	}

	SECTION("Insert All") {
		double items1[] = { 1.3, 5.3, 1.7, 6.0, 3.12 };
		double items1_sort[] = { 1.3, 1.7, 3.12, 5.3, 6.0 };
		sv_set<double> c1;
		
		for (int i = 0; i < 5; i++) {
			REQUIRE(c1.insert(items1[i]).second);
		}

		int i = 0;
		for (auto item : c1) {
			REQUIRE(item == items1_sort[i++]);
		}
	}

	SECTION("Regular") {
		double items1[] = { 2.3, 4.5, 6.7, 7.7, 9.5 };
		sv_set<double> c1(sv_set<double>::ordered_and_unique_range{}, items1, 5);
		c1.reserve(10);

		// insert at the front of array
		c1.insert(1.2);
		REQUIRE(*c1.begin() == 1.2);

		// insert in the middle
		c1.insert(6.9);
		REQUIRE(c1.find(6.9) != c1.end());

		// insert at the back
		c1.insert(10.7);
		REQUIRE(*(c1.end() - 1) == 10.7);
		REQUIRE(c1.size() == 8);
		REQUIRE(c1.capacity() == 10);
	}

	SECTION("Greater Comparator") {
		sv_set<double, std::greater<double>> c1;
		c1.insert(12.3);
		c1.insert(1.2);
		c1.insert(14.3);
		c1.insert(0.3);

		REQUIRE(*c1.begin() == 14.3);
		REQUIRE(*(c1.end() - 1) == 0.3);
		REQUIRE(c1.size() == 4);
		REQUIRE(c1.capacity() == 4);
	}
}
