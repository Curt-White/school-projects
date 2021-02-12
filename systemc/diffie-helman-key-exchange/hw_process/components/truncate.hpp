#ifndef TRUNCATE_HPP
#define TRUNCATE_HPP

#include "systemc.h"
#include "../../digit.h"

SC_MODULE(truncator) {
	sc_in <NN_DIGIT> in;
	sc_out <NN_HALF_DIGIT> out;

	void truncate() {
		out.write((NN_HALF_DIGIT)in.read());
	}

	SC_CTOR(truncator) {
		SC_METHOD(truncate); sensitive << in;
	}
};

#endif
