#include "systemc.h"

#define RDBLK 0
#define RDBYT 1
#define WTBYT 2
#define WTBLK 3

static sc_logic Z[8] = { SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z,
							SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z };

SC_MODULE(mem_ctrl) {
// Ports
	sc_inout <sc_lv<8> > data; // Bidirectional port of logic-vector type
	sc_in < sc_uint<8> > addr;
	sc_in < sc_uint<2> > comm;
	sc_in <bool> new_comm; // Input port signaling “start”
	sc_out <bool> complete; // Output port signaling “done”
	sc_in_clk clk;
	sc_in <bool> reset;
	sc_lv <8> memory[256]; // Internal memory space (256 8-bit words)

	void mem_process();

	SC_CTOR(mem_ctrl) {
		SC_CTHREAD(mem_process, clk.pos());
	}
};