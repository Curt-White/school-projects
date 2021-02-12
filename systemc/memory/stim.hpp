#include "systemc.h"

SC_MODULE(stim) {
	sc_inout <sc_lv<8> > data; // Bidirectional port of logic-vector type
	sc_out < sc_uint<8> > addr;
	sc_out < sc_uint<2> > comm;
	sc_out <bool> new_comm; // Input port signaling “start”
	sc_in <bool> complete; // Output port signaling “done”
	sc_in_clk clk;
	sc_out <bool> reset;

	void run();

	SC_CTOR(stim) {
		SC_CTHREAD(run, clk.pos());
	}
};