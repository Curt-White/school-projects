#include "systemc.h"

template <class T>
struct eq2: public sc_module {
	sc_in <T> in1, in2;
	sc_out <bool> out;

	void eq_op() {
		out.write(in1 == in2);
	}

	SC_HAS_PROCESS(eq2);
	eq2(sc_module_name name): sc_module(name) {
		SC_METHOD(eq_op); sensitive << in1 << in2;
	}
};
