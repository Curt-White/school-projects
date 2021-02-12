#ifndef LAND2_HPP
#define LAND2_HPP

#include "systemc.h"

struct logic_and2: public sc_module {
	sc_in <bool> in1, in2;
	sc_out <bool> out;

	void logic_and2_op() {
		out.write(in1 && in2);
	}

	SC_HAS_PROCESS(logic_and2);
	logic_and2(sc_module_name name): sc_module(name) {
		SC_METHOD(logic_and2_op); sensitive << in1 << in2;
	}
};

#endif
