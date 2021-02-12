#include "systemc.h"
#include "producer.h"
#include "consumer.h"
#include "fifo.h"

int sc_main(int argc, char* argv[]) {
	consumer<int> c1("c1");
	producer<int> p1("p1");
	fifo<int, 4> f1("f1");	// fifo template arg 4 is the size

	sc_clock clk_fast("clk_fast", 10, SC_NS); // producer clock
	sc_clock clk_slow("clk_slow", 20, SC_NS); // consumer clock

	p1.clk(clk_fast);
	p1.out(f1);

	c1.clk(clk_slow);
	c1.in(f1);

	sc_start(250, SC_NS);
}