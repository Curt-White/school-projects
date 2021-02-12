#include "systemc.h"

SC_MODULE (adder) {
	sc_in <float> a, b;
	sc_out <float> c;

	void add() {
		c.write(a.read() + b.read());
	}
	
	SC_CTOR(adder) {
		SC_METHOD(add); sensitive << a << b;	
	}

};
