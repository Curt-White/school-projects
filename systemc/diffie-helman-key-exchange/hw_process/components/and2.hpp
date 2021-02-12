#ifndef AND2_HPP
#define AND2_HPP

#include "systemc.h"

template <class T>
struct and2: public sc_module {
	sc_in <T> in1, in2;
	sc_out <T> out;

	void and_op() {
		out.write(in1 & in2);
	}

	SC_HAS_PROCESS(and2);
	and2(sc_module_name name): sc_module(name) {
		SC_METHOD(and_op); sensitive << in1 << in2;
	}
};

#endif
