#ifndef PRODUCER_H
#define PRODUCER_H

#include "systemc.h"
#include "fifo_if.h"

template <class T> class producer : public sc_module {
	public:
	sc_port < fifo_out_if<T> > out;
	sc_in_clk clk;

	void do_writes() {
		T data = 0;
		// constantly enumerate the numbers from 0 to 9
		while (true) {
			if (out->write(data)) { 
				cout << "W " << data << " at " << sc_time_stamp() << endl;
				data = (data + 1) % 10;
			}

			wait();
		}
	}

	SC_HAS_PROCESS(producer);
	producer(sc_module_name name): sc_module(name) {
		SC_THREAD(do_writes); sensitive << clk.pos(); dont_initialize();
	}
};

#endif