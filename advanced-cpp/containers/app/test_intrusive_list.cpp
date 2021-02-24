#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ra/intrusive_list.hpp"

struct Widget {
	Widget(int value_) : value(value_) {}
	int value;
	ra::intrusive::list_hook hook;
};

using widget_list =  ra::intrusive::list<Widget, &Widget::hook>;

TEST_CASE("Construction") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1;

	l1.push_back(a);
	l1.push_back(b);
	l1.push_back(c);
	l1.push_back(d);

	REQUIRE(l1.size() == 4);
	widget_list l2;
	l2 = std::move(l1);

	REQUIRE(l1.size() == 0);
	REQUIRE(l1.begin() == l1.end());

	REQUIRE(l2.size() == 4);

	widget_list l3(std::move(l2));

	REQUIRE(l2.size() == 0);
	REQUIRE(l2.begin() == l2.end());

	REQUIRE(l3.size() == 4);
}

TEST_CASE("Random Calls") {
	const widget_list l1;
	widget_list l2;
	auto b = l1.begin();
	auto c = l1.end();

	auto d = l2.end();
	REQUIRE(b != d);
}

TEST_CASE("Push Back") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1;

	l1.push_back(a);
	l1.push_back(b);
	l1.push_back(c);
	l1.push_back(d);
	auto it = l1.begin();
	
	REQUIRE(it->value == 32);
	REQUIRE((++it)->value == 45);
	REQUIRE((it++)->value == 45);
	REQUIRE((it++)->value == 56);
	REQUIRE((it++)->value == 12);
	REQUIRE(it == l1.end());
	REQUIRE(it != l1.begin());

	REQUIRE((--it)->value == 12);
	REQUIRE((--it)->value == 56);
	REQUIRE((--it)->value == 45);
	REQUIRE((--it)->value == 32);
	REQUIRE(l1.size() == 4);
}

TEST_CASE("Pop Back") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1;
		
	l1.push_back(a);
	l1.push_back(b);
	l1.push_back(c);
	l1.push_back(d);

	auto it = l1.end();
	l1.pop_back();
	it--;
	REQUIRE(it->value == 56);
	REQUIRE(++it == l1.end());
	it--;
	REQUIRE((--it)->value == 45);
	REQUIRE(l1.size() == 3);
}

TEST_CASE("Test Clear") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1;
		
	l1.push_back(a);
	l1.push_back(b);
	l1.push_back(c);
	l1.push_back(d);

	l1.clear();
	REQUIRE(l1.size() == 0);
	REQUIRE(l1.end() == l1.begin());
}

TEST_CASE("Test Erase") {
	Widget a(32), b(45), c(56), d(12), e(23);
	widget_list l1;
		
	l1.push_back(a);
	l1.push_back(b);
	l1.push_back(c);
	l1.push_back(d);
	l1.push_back(e);

	// remove item at the front
	auto ret_it = l1.erase(l1.begin());
	REQUIRE(ret_it == l1.begin());
	REQUIRE(ret_it->value == 45);
	REQUIRE((++ret_it)->value == 56);
	REQUIRE(l1.size() == 4);

	// remove item at the end
	ret_it = l1.erase(--l1.end());
	REQUIRE(ret_it == l1.end());
	REQUIRE((--ret_it)->value == 12);

	// remove item in the middle
	auto middle = ++l1.begin();
	ret_it = l1.erase(middle);
	REQUIRE(ret_it->value == 12);
	REQUIRE((--ret_it)->value == 45);
	REQUIRE(l1.size() == 2);
}

TEST_CASE("Test Back") {
	Widget a(32), b(45), c(56), d(12), e(23);
	widget_list l1;
	l1.push_back(a);

	auto const_test = [](const widget_list &r) {
		REQUIRE(r.back().value == 32);
	};

	REQUIRE(l1.back().value == 32);
	const_test(l1);
}

TEST_CASE("Test Insert") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1;

	// instert into empty list
	auto res_iter = l1.insert(l1.begin(), a);
	REQUIRE(l1.begin()->value == 32);
	REQUIRE((++res_iter) == l1.end());
	REQUIRE((--l1.end())->value == 32);
	REQUIRE(l1.size() == 1);

	// insert at the beginning
	res_iter = l1.insert(l1.begin(), b);
	REQUIRE(l1.begin()->value == 45);
	REQUIRE(l1.begin() == res_iter);
	REQUIRE((++res_iter)->value == 32);
	REQUIRE(l1.size() == 2);

	// insert at the end
	res_iter = l1.insert(l1.end(), c);
	REQUIRE(res_iter->value == 56);
	REQUIRE((++res_iter) == l1.end());
	REQUIRE((--(--res_iter))->value == 32);
	REQUIRE(l1.size() == 3);

	// insert in the middle
	res_iter = l1.insert(++l1.begin(), d);
	REQUIRE(res_iter->value == 12);
	REQUIRE((++res_iter)->value == 32);
	REQUIRE((--(--res_iter))->value == 45);
	REQUIRE(l1.size() == 4);
}

TEST_CASE("Test Swap") {
	Widget a(32), b(45), c(56), d(12);
	widget_list l1, l2;

	l1.push_back(a);
	l2.push_back(b);
	l2.push_back(c);
	l1.swap(l2);

	REQUIRE(l1.size() == 2);
	REQUIRE(l2.size() == 1);

	REQUIRE(l1.begin()->value == 45);
	REQUIRE((--l1.end())->value == 56);
	REQUIRE(l2.begin()->value == 32);
}
