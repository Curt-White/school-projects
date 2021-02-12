#ifndef SUB2_HPP
#define SUB2_HPP

#include "systemc.h"

template <class T>
struct sub2: public sc_module {
	sc_in <T> in1, in2;
	sc_out <T> out;

	void subtract() {
		out.write(in1 - in2);
	}

	SC_HAS_PROCESS(sub2);
	sub2(sc_module_name name): sc_module(name) {
		SC_METHOD(subtract); sensitive << in1 << in2;
	}
};

#endif
