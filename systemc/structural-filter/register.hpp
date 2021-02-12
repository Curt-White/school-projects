#include "systemc.h"

SC_MODULE (reg) {
	sc_in <float> in;
	sc_in <bool> reset;
	sc_out <float> out;
	sc_in_clk clk;

	void loop() {
		out.write(0);
		wait();
		
		while (1) {
			out.write(in.read());
			wait();
		}
	}

	SC_CTOR(reg) {
		SC_CTHREAD(loop, clk.pos());
		reset_signal_is(reset, true);
	}
};
