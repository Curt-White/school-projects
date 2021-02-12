#include "systemc.h"
#include "memory.hpp"
#include "stim.hpp"

int sc_main(int argc, char* argv[]) {
    // Memory module instance:
    mem_ctrl MC("MC");
    
    // Your testbench module(s) instances:
    stim gen("gen");


    // Internal signals:
    sc_signal <bool> reset, new_comm, complete;
    sc_signal < sc_uint<2> > comm;
    sc_signal < sc_uint<8> > addr;
    sc_signal_rv <8> data;
    sc_clock clk("aclock", 10, SC_NS, 0.5, 1, SC_NS);
    // _rv needed because of multiple drivers
  
    // Interconnects:
    // connect eh memory controller
    MC.clk(clk);
    MC.reset(reset);
    MC.data(data);
    MC.addr(addr);
    MC.comm(comm);
    MC.new_comm(new_comm);
    MC.complete(complete);

    // connect the testbench
    gen.clk(clk);
    gen.reset(reset);
    gen.data(data);
    gen.addr(addr);
    gen.comm(comm);
    gen.new_comm(new_comm);
    gen.complete(complete);

    // setup trace file and include all signals
    sc_trace_file *tf = sc_create_vcd_trace_file("filter_trace");
	tf -> set_time_unit(1, SC_NS);

	sc_trace(tf, clk, "Clock");
    sc_trace(tf, reset, "Reset");
	sc_trace(tf, data, "Data");
	sc_trace(tf, addr, "Address");
    sc_trace(tf, comm, "Command");
    sc_trace(tf, new_comm, "New Command");
    sc_trace(tf, complete, "Complete");

	sc_start();
	sc_close_vcd_trace_file(tf);

    return 0;
}