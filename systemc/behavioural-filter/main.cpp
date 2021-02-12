#include "systemc.h"
#include "stim.hpp"
#include "mon.hpp"
#include "filter_b.hpp"


int sc_main(int argc, char **argv) {
	sc_signal <float> x, y;
	sc_signal <bool> reset;
	sc_clock clk("test_clock", 10, SC_NS, 0.5, 1, SC_NS);

	filter_b tfb("tfb");
	tfb.x(x); tfb.y(y); tfb.reset(reset); tfb.clk(clk);

	stim_gen stg("stim");
	stg.x(x); stg.reset(reset); stg.clk(clk);

	monitor mn("mn");
	mn.x(x); mn.y(y); mn.reset(reset); mn.clk(clk);

	sc_trace_file *tf = sc_create_vcd_trace_file("filter_trace");
	tf -> set_time_unit(1, SC_NS);

	sc_trace(tf, clk, "Clock");
	sc_trace(tf, x, "X");
	sc_trace(tf, y, "Y");
	sc_trace(tf, reset, "Reset");

	sc_start();
	sc_close_vcd_trace_file(tf);

	return 0;
}
