#ifndef GTEQ_HPP
#define GTEQ_HPP

#include "systemc.h"

template <class T>
struct gteq: public sc_module {
	sc_in <T> in1, in2;
	sc_out <bool> out;

	void check_greater_eq() {
		out.write(in1 >= in2);
	}

	SC_HAS_PROCESS(gteq);
	gteq(sc_module_name name): sc_module(name) {
		SC_METHOD(check_greater_eq); sensitive << in1 << in2;
	}
};

#endif
