#ifndef RSHIFT_HPP
#define RSHIFT_HPP

#include "systemc.h"

template <class T, int SHIFT>
struct right_shift : public sc_module {
	sc_in <T> in;
	sc_out <T> out;

	void shift() {
		out.write(in.read() >> SHIFT);
	}

	SC_HAS_PROCESS(right_shift);
	right_shift(sc_module_name name): sc_module(name) {
		SC_METHOD(shift); sensitive << in;
	}
};

#endif
