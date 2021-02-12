#include "systemc.h"


SC_MODULE(filter_b) {
	sc_in <bool> reset;
	sc_in <float> x;
	sc_out <float> y;
	
	sc_in_clk clk;

	sc_signal <float> r0_in, r0_out, r1_in, r1_out, r2_out;

	// Update the value of the output signal y
	void output();

	// Update the values of the registers inputs
	void reg_input();

	// Update the output values of the registers, clocked
	void update();

	SC_CTOR(filter_b) {
		SC_METHOD(output); 		sensitive << r0_out << r1_out << r2_out;
		SC_METHOD(reg_input);		sensitive << r0_out << r1_out << r2_out << x;
		SC_CTHREAD(update, clk.pos());

		reset_signal_is(reset, true);
	}
};


