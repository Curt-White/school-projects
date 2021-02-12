#include "systemc.h"

SC_MODULE(multiplier) {
	sc_in <float> a, b;
	sc_out <float> c;

	void multiply() {
		c.write(a.read() * b.read());
	}	


	SC_CTOR(multiplier) {
		SC_METHOD(multiply); sensitive << a << b;	
	}
};
