#ifndef LOR2_HPP
#define LOR2_HPP

#include "systemc.h"

struct logic_or2: public sc_module {
	sc_in <bool> in1, in2;
	sc_out <bool> out;

	void logic_or2_op() {
		out.write(in1 || in2);
	}

	SC_HAS_PROCESS(logic_or2);
	logic_or2(sc_module_name name): sc_module(name) {
		SC_METHOD(logic_or2_op); sensitive << in1 << in2;
	}
};

#endif
