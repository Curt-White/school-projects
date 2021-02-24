#define CATCH_CONFIG_MAIN

#include <iostream>

#include "catch2/catch.hpp"
#include "sim/quad.hpp"
#include "sim/shapes.hpp"

TEST_CASE("Insert") {
	SECTION("Insert First Level") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		items.push_back(sim::Rectangle(1, 1));
		items.push_back(sim::Rectangle(2, 2));
		items.push_back(sim::Rectangle(3, 3));
		items.push_back(sim::Rectangle(4, 4));
		for (int i = 0; i < items.size(); i++) { 
			tree.insert(&items[i]);
		}
		REQUIRE(tree.size() == 4);
	}

	SECTION("Insert Split 2 Levels") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		items.push_back(sim::Rectangle(1, 1));
		items.push_back(sim::Rectangle(50, 50));
		items.push_back(sim::Rectangle(4, 64));
		items.push_back(sim::Rectangle(64, 4));
		items.push_back(sim::Rectangle(91, 91));
		for (int i = 0; i < items.size(); i++) { 
			tree.insert(&items[i]);
		}

		REQUIRE(tree.size() == 5);
		REQUIRE(tree.level() == 2);
		REQUIRE(tree.size_level() == 1);
		REQUIRE(tree.get_top_left()->size() == 1);
		REQUIRE(tree.get_top_right()->size() == 1);
		REQUIRE(tree.get_bottom_left()->size() == 1);
		REQUIRE(tree.get_bottom_right()->size() == 1);
	}

	SECTION("Insert to Full") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		for (int i = 0; i < 6; i++) items.push_back(sim::Rectangle(1, 1));
		for (int i = 0; i < 5; i++) REQUIRE_NOTHROW(tree.insert(&items[i]));
		REQUIRE_THROWS(tree.insert(&items[5]));
		REQUIRE(tree.size() == 5);
	}
}

TEST_CASE("Test Search") {
	sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
	std::vector<sim::Rectangle> items;
	items.push_back(sim::Rectangle(1, 1));
	items.push_back(sim::Rectangle(2, 2));
	items.push_back(sim::Rectangle(3, 3));
	items.push_back(sim::Rectangle(50, 50));
	items.push_back(sim::Rectangle(4, 64));
	items.push_back(sim::Rectangle(64, 4));
	items.push_back(sim::Rectangle(91, 91));
	for (int i = 0; i < items.size(); i++) tree.insert(&items[i]);

	// test fetching when on the border
	std::vector<sim::Rectangle*> matches1;
	sim::Rectangle bound = sim::Rectangle(50, 50);
	tree.search(&bound, matches1);
	REQUIRE(matches1.size() == 7);

	//top left quadrant test
	std::vector<sim::Rectangle*> matches2;
	sim::Rectangle bound2 = sim::Rectangle(4, 4);
	tree.search(&bound2, matches2);
	REQUIRE(matches2.size() == 4);

	// bottom left quad test
	std::vector<sim::Rectangle*> matches3;
	sim::Rectangle bound3 = sim::Rectangle(4, 64);
	tree.search(&bound3, matches3);
	REQUIRE(matches3.size() == 2);
}

TEST_CASE("Test Iterator") {
	SECTION("2-Level Quad Tree Iterator") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		items.push_back(sim::Rectangle(1, 1));
		items.push_back(sim::Rectangle(50, 50));
		items.push_back(sim::Rectangle(4, 64));
		items.push_back(sim::Rectangle(64, 4));
		items.push_back(sim::Rectangle(91, 91));
		items.push_back(sim::Rectangle(8, 16));
		items.push_back(sim::Rectangle(4, 5));
		items.push_back(sim::Rectangle(14, 12));
		for (int i = 0; i < items.size(); i++) tree.insert(&items[i]);

		auto it = tree.begin();
		REQUIRE(*(*it) == sim::Rectangle(50, 50)); ++it;
		REQUIRE(*(*it)== sim::Rectangle(64, 4)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(1, 1)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(8, 16)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(4, 5)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(14, 12)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(4, 64)); ++it;
		REQUIRE(*(*it) == sim::Rectangle(91, 91)); ++it;
		REQUIRE(it == tree.end());
	}

	SECTION("Iterator Copying and Conversions") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		items.push_back(sim::Rectangle(1, 1));
		items.push_back(sim::Rectangle(50, 50));
		for (int i = 0; i < items.size(); i++) tree.insert(&items[i]);

		auto it = tree.begin(); ++it;
		auto it2 = it;

		REQUIRE(it == it2);
		it2++; it++;

		REQUIRE(it == it2);
		REQUIRE(*it == *it2);
	}

	SECTION("Constant Iterators") {
		sim::QuadTree<sim::Rectangle*, 5, 5> tree(sim::Rectangle(0, 0, 100, 100));
		std::vector<sim::Rectangle> items;
		items.push_back(sim::Rectangle(1, 1));
		items.push_back(sim::Rectangle(50, 50));
		for (int i = 0; i < items.size(); i++) tree.insert(&items[i]);

		sim::QuadTree<sim::Rectangle*, 5, 5>::const_iterator c_it(tree.begin());
		REQUIRE(tree.begin() == c_it);
	}
}

TEST_CASE("Test Shape Collision Checking") {
	sim::Rectangle r1(0, 0, 20, 20), // initial item
				   r2(1, 1, 5, 5), 	// an item completely inside the above
				   r3(15, 15, 20, 20), // partially eclipsing item
				   r4(40, 40, 1, 1);

	SECTION("Contains") {
		REQUIRE(r1.is_containing(r2));
		REQUIRE(!r2.is_containing(r1));
		REQUIRE(!r1.is_containing(r1));
		REQUIRE(!r1.is_containing(r3));
	}

	SECTION("Intesection") {
		REQUIRE(r1.is_intersecting(r1));
		REQUIRE(!r1.is_intersecting(r4));
		REQUIRE(r1.is_intersecting(r3));
		REQUIRE(r1.is_intersecting(r2));
		REQUIRE(r2.is_intersecting(r1));
	}
}

TEST_CASE("Rectangles") {
	sim::Rectangle r1(0, 0, 20, 20), // initial item
				   r2(1, 1, 5, 5), 	// an item completely inside the above
				   r3(15, 15, 20, 20), // partially eclipsing item
				   r4(40, 40, 1, 1);

	SECTION("Equals") {
		REQUIRE(r1 == r1);
		REQUIRE(!(r1 == r3));
	}

	SECTION("Accessors") {
		REQUIRE(r1.x() == 0);
		REQUIRE(r1.y() == 0);
		REQUIRE(r1.height() == 20);
		REQUIRE(r1.width() == 20);
	}
}
