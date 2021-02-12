#ifndef ADD2_HPP
#define ADD2_HPP

#include "systemc.h"

template <class T>
struct add2: public sc_module {
	sc_in <T> in1, in2;
	sc_out <T> out;

	void add_op() {
		out.write(in1 + in2);
	}

	SC_HAS_PROCESS(add2);
	add2(sc_module_name name): sc_module(name) {
		SC_METHOD(add_op); sensitive << in1 << in2;
	}
};

#endif
