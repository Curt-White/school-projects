#include "systemc.h"

#ifndef GT_HPP
#define GT_HPP

template <class T>
struct gt: public sc_module {
	sc_in <T> in1, in2;
	sc_out <bool> out;

	void check_greater() {
		out.write(in1 > in2);
	}

	SC_HAS_PROCESS(gt);
	gt(sc_module_name name): sc_module(name) {
		SC_METHOD(check_greater); sensitive << in1 << in2;
	}
};

#endif
