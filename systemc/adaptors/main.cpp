#include "systemc.h"
#include "adder.hpp"
#include "fork.hpp"
#include "generator.hpp"
#include "printer.hpp"

#include "hw_fifo.hpp"
#include "read_adapt.hpp"
#include "write_adapt.hpp"

int sc_main (int argc, char* argv[]) {
	DF_Const <int> constant ("constant", 1); // constant = 1
	DF_Adder <int> adder ("adder");
	DF_Fork <int> fork("fork");
	DF_Printer <int> printer ("printer", 10); // 10 interations

	sc_fifo <int> const_out ("const_out", 1); // FIFO of size 1
	sc_fifo <int> printer_in ("printer_in", 1); // FIFO of size 1
	sc_fifo <int> feedback ("feedback", 1); // FIFO of size 1
	hw_fifo <int> adder_out("adder_out", 1);

	FIFO_READ_HS <int> read_adapter("read_adapter");

	sc_clock clk("clk", 10, SC_NS);
	sc_signal <bool> adder_valid("adder_valid"), adder_ready("adder_ready"), 
		fork_valid("fork_valid"), fork_ready("fork_ready");
	sc_signal <int> adder_data("adder_data"), fork_data("fork_data");

	adder.clock(clk);
	adder.data(adder_data);
	adder.input1 (const_out); 
	adder.input2 (feedback); 
	adder.ready(adder_ready);
	adder.valid(adder_valid);

	read_adapter.clock(clk);
	read_adapter.data(fork_data);
	read_adapter.valid(fork_valid);
	read_adapter.ready(fork_ready);

	adder_out.clk(clk);
	adder_out.data_in(adder_data);
	adder_out.data_out(fork_data);
	adder_out.ready_out(adder_ready);
	adder_out.ready_in(fork_ready);
	adder_out.valid_in(adder_valid);
	adder_out.valid_out(fork_valid);

	feedback.write (40); // do not forget about initialization!
	constant.output (const_out); printer.input (printer_in);
	
	fork.input (read_adapter); fork.output1 (feedback); fork.output2 (printer_in);
	// Start simulation without time limit
	// The printer module is responsible for stopping the simulation
	sc_start (5000, SC_NS);
	return 0;
}
