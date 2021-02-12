#ifndef MUX2_HPP
#define MUX2_HPP

#include "systemc.h"

template <class T>
struct mux2: public sc_module {
	// select true is in1
	sc_in <T> in1, in2;
	sc_in <bool> select;
	sc_out <T> out;

	void mux_select() {
		if (select.read()) {
			out.write(in1.read());
		} else {
			out.write(in2.read());
		}
	}

	SC_HAS_PROCESS(mux2);
	mux2(sc_module_name name): sc_module(name) {
		SC_METHOD(mux_select); sensitive << in1 << in2;
	}
};

#endif
