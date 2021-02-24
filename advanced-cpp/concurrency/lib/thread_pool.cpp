#include "ra/thread_pool.hpp"
#include <thread>
#include <algorithm>

#include <iostream>

namespace ra::concurrency {
	int hw_threads(int alt) { 
		auto t = std::thread::hardware_concurrency();
		return t ? t : alt;
	}

	thread_pool::thread_pool(): is_shutdown_(false), num_threads_(hw_threads(2)), tasks_(32),
		idle_threads_(0) {
		threads_ = new std::thread[num_threads_];
		int id = 0;
		std::generate_n(threads_, num_threads_, [&] () -> std::thread { return std::thread(make_thread_handler(id++)); } );
	}

	thread_pool::thread_pool(std::size_t num_threads): is_shutdown_(false), num_threads_(num_threads), 
		tasks_(32), idle_threads_(0) {
		assert(num_threads_ > 0);
		threads_ = new std::thread[num_threads_];
		// generate enumerated ids for all of the threads, mostly for testing
		int id = 0;
		std::generate_n(threads_, num_threads_, [&] () -> std::thread { return std::thread(make_thread_handler(id++)); });
	}

	thread_pool::~thread_pool() {
		if (!is_shutdown_) {
			shutdown();
		}

		// make sure all of the threads are unjoinable otherwise will throw
		for (int i = 0; i < num_threads_; i++) {
			if (threads_[i].joinable()) threads_[i].join();
		}

		delete[] threads_;
	}

	std::function<void()> thread_pool::make_thread_handler(int thread_id) {
		return [this, thread_id] {
			while (1) {
				task next;

				{	// need to drop the lock before reaching end brackets
					std::unique_lock lock(task_mut_);
					if (tasks_.is_empty()) {
						new_jobs_.wait(lock, [&] () -> bool  {
							return !tasks_.is_empty() || is_shutdown_;
						});

						// if there are not jobs and shutdown then exit thread
						if (tasks_.is_empty() && is_shutdown_) {
							++idle_threads_;
							all_done_.notify_one();
							return;
						}
					}

					tasks_.pop(next);
					can_push.notify_one();
				}

				next();
			}
		};
	}

	void thread_pool::schedule(std::function<void()>&& func) {
		if (is_shutdown_) return;

		// There is a full extra layer of locking which effectively makes the multi-threaded
		// queue useless since we rely on check is_empty alot and it is not thread safe.
		std::unique_lock lock(task_mut_);
		if (tasks_.is_full()) {
			can_push.wait(lock, [this] () -> bool { return !this->tasks_.is_full(); });
		}

		tasks_.push(std::move(func));
		new_jobs_.notify_one();
	}

	void thread_pool::shutdown() {
		if (is_shutdown_) return;

		std::unique_lock lock(task_mut_);
		is_shutdown_ = true;

		new_jobs_.notify_all(); // wake up all to clean up remaining tasks
		all_done_.wait(lock, [this] () -> bool  {
			// if there are not tasks and all threads are idle
			return (this->tasks_.is_empty() && (this->idle_threads_ == this->num_threads_));
		});
	}
}
