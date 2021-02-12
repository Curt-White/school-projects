#ifndef EXPAND_HPP
#define EXPAND_HPP

#include "systemc.h"
#include "../../digit.h"

SC_MODULE(expander) {
	sc_in <NN_HALF_DIGIT> in;
	sc_out <NN_DIGIT> out;

	void expand() {
		out.write((NN_DIGIT)in.read());
	}

	SC_CTOR(expander) {
		SC_METHOD(expand); sensitive << in;
	}
};

#endif
