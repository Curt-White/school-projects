#ifndef CONSUMER_H
#define CONSUMER_H

#include "systemc.h"
#include "fifo_if.h"

template <class T> class consumer : public sc_module {
	public:
	sc_port < fifo_in_if<T> > in;
	sc_in_clk clk;
	
	void do_reads() {
		T data;
		// constantly attempt to read from the queue
		while (true) {
			if (in->read(data)){ 
				cout << "R " << data << " at " << sc_time_stamp() << endl;
			}
			
			wait();
		}
	} 

	SC_HAS_PROCESS(consumer);
	consumer(sc_module_name name): sc_module(name) {
		SC_THREAD(do_reads); sensitive << clk.pos(); dont_initialize();
	}
};

#endif