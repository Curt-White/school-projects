#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"
#include "ra/thread_pool.hpp"
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>

std::mutex some_mut;

template <class T>
void lock_print(T out) {
	std::unique_lock lock(some_mut);
	std::cout << out << std::endl;
}

TEST_CASE("Four Threaded") {
	std::atomic<int> count = 50;
	ra::concurrency::thread_pool pool(4);
	REQUIRE(pool.size() == 4);

	for (int i = 0; i < 50; i++) {
		pool.schedule([i, &count] () {
			count --;
		});
	}

	pool.shutdown();
	REQUIRE(pool.is_shutdown());
	REQUIRE(count == 0);
}
