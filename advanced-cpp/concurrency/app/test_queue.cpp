#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "ra/queue.hpp"
#include <thread>
#include <iostream>
#include <mutex>
#include <sstream>
#include <atomic>

using int_queue = ra::concurrency::queue<int>;
std::mutex m;

template <class T>
void lock_print(T out) {
	std::unique_lock lock(m);
	std::cout << out << std::endl;
}

TEST_CASE("Single threaded access") {
	int_queue q(10);
	for (int i = 0; i < 10; i++) {
		REQUIRE(q.push(std::move(i)) == int_queue::status::success);
	}

	REQUIRE(q.is_full());
	REQUIRE(!q.is_empty());
	int x = -1;
	for (int i = 0; i < 10; i++) {
		REQUIRE(q.pop(x) == int_queue::status::success);
		REQUIRE(x == i);
	}

	REQUIRE(!q.is_full());
	REQUIRE(q.is_empty());
}

TEST_CASE("fast writer accesses") {
	int_queue q(10);

	std::thread t1([&] {
		for (int i = 0; i < 20; i += 2) {
			REQUIRE(q.push(std::move(i)) == int_queue::status::success);
			REQUIRE(q.push(std::move(i + 1)) == int_queue::status::success);
		}
	});

	std::thread t2([&] {
		int x = -1;
		int i = 0;
		while (i < 20) {
			REQUIRE(q.pop(x) == int_queue::status::success);
			REQUIRE(x == i);
			i++;
		}
	});

	t1.join();
	t2.join();
}

TEST_CASE("fast reader accesses") {
	int_queue q(10);

	std::thread t1([&] {
		for (int i = 0; i < 20; i ++) {
			REQUIRE(q.push(std::move(i)) == int_queue::status::success);
		}
	});

	std::thread t2([&] {
		int x = -1;
		for (int i = 0; i < 10; i++) {
			REQUIRE(q.pop(x) == int_queue::status::success);
			REQUIRE(q.pop(x) == int_queue::status::success);
		}
	});

	t1.join();
	t2.join();
}

TEST_CASE("mutltiple popping threads") {
	// int_queue q(10);
	// std::atomic<int> popped = 0;

	// std::thread t1([&] {
	// 	for (int i = 0; i < 20; i ++) {
	// 		q.push(std::move(i));
	// 	}
	// });

	// std::thread t2([&popped, &q] {
	// 	int x = 0;
	// 	while (popped < 20) {
	// 		q.pop(x);
	// 		popped++;
	// 		// std::stringstream s;
	// 		// s << "T2: " << popped << " " << x << std::endl; 
	// 		// lock_print(s.str());
	// 	}
	// });

	// std::thread t3([&] {
	// 	int x = 0;
	// 	while (popped < 20) {
	// 		q.pop(x);
	// 		popped++;
	// 		// std::stringstream s;
	// 		// s << "T3: " << popped << " " << x << std::endl; 
	// 		// lock_print(s.str());
	// 	}
	// });

	
	// t1.join(); t2.join(); t3.join();
	// REQUIRE(popped == 20);
}
