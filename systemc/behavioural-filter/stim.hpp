#include "systemc.h"

SC_MODULE(stim_gen) {
	sc_out <float> x;
	sc_out <bool> reset;
	
	sc_in_clk clk;

	void gen() {
		reset.write(true); wait();
		x.write(1); reset.write(false); wait();
		x.write(0); wait();
		
		// wait another 9 clock cycles to make it 12
		for (int i = 0; i < 9; i++) {
			wait();
		}

		sc_stop();
	}

	SC_CTOR(stim_gen) {
		SC_CTHREAD(gen, clk.pos());
	}
};

