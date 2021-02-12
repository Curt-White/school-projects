#ifndef REGISTER_HPP
#define REGISTER_HPP

#include "systemc.h"

template <class T>
struct reg : public sc_module {
	sc_in <T> in;
	sc_out <T> out;
	sc_in <bool> load;
	sc_in_clk clk;

	void loop() {
		while (1) {
			if (load.read()) {
				out.write(in.read());
			}

			wait();
		}
	}

	SC_HAS_PROCESS(reg);
	reg(sc_module_name name): sc_module(name) {
		SC_CTHREAD(loop, clk.pos());
	}
};

#endif
