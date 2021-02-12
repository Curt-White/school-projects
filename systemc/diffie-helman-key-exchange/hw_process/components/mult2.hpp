#ifndef MULT2_HPP
#define MULT2_HPP

#include "systemc.h"

template <class T>
struct mult2: public sc_module {
	sc_in <T> in1, in2;
	sc_out <T> out;

	void mult() {
		out.write(in1 * in2);
	}

	SC_HAS_PROCESS(mult2);
	mult2(sc_module_name name): sc_module(name) {
		SC_METHOD(mult); sensitive << in1 << in2;
	}
};

#endif
