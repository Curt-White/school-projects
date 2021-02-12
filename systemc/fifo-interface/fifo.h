#ifndef FIFO_H
#define FIFO_H

#include "systemc.h"
#include "fifo_if.h"

template <class T, int S> class fifo : public sc_module, public fifo_out_if <T>, public fifo_in_if <T> {
	private:
	
	int size_;
	int front_;
	int num_items_;
	T data_[S];

	sc_event read_request_, write_request_, done_;
	bool read_flag_, write_flag_, read_status_, write_status_;
	T *read_item_;
	T write_item_;

	public:

	// write if the queue is not full else return error (false)
	bool write(T data) {
		write_item_ = data;
		write_flag_ = true;
		write_request_.notify();
		wait(done_);

		return write_status_;
	}

	// read if the queue is not empty else return error (false)
	bool read(T& data) {
		read_item_ = &data;
		read_flag_ = true;
		read_request_.notify();
		wait(done_);

		return read_status_;
	}

	// arbitrates access to the fifo items and ensures orders of requests
	void arbitrator() {
		while (1) {
			// todo if need to read from the queue
			if (read_flag_) {
				read_flag_ = false;
				// set status to false signaling an error if the array is empty
				if (num_items_ == 0) {
					read_status_ = false;
				} else {
					*read_item_ = data_[front_];
					front_ = (front_ + 1) % size_;
					num_items_--;
					read_status_ = true;
				}
			}

			// things to do if need to write from the queue
			if (write_flag_) {
				write_flag_ = false;
				// set status to false signaling an error if the array is full
				if (num_items_ == size_) {
					write_status_ = false;
				} else {
					data_[(front_ + num_items_) % size_] = write_item_;
					num_items_++;
					write_status_ = true;
				}
			}

			done_.notify();
			wait();
		}
	}

	SC_HAS_PROCESS(fifo);
	fifo(sc_module_name name): sc_module(name), size_(S), front_(0), num_items_(0), read_flag_(false),
		write_flag_(false), read_status_(false), write_status_(false) {

		SC_THREAD(arbitrator); sensitive << read_request_ << write_request_;
	}
};

#endif