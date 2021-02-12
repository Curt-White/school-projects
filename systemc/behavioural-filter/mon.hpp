#include "systemc.h"
#include <iomanip>

SC_MODULE(monitor) {
	sc_in <float> x;
	sc_in <float> y;
	sc_in <bool> reset;
	sc_in_clk clk;

	void print() {
		std::cout << std::setw(10) << "Time";
		std::cout << std::setw(15) << "X" << std::setw(15) << "Y";
		std::cout << std::setw(10) << "Reset" << std::endl;

		while (true) {
			std::cout << std::setw(10) << sc_time_stamp();
			std::cout << std::setw(15) << x.read() << std::setw(15) << y.read();
			std::cout << std::setw(10) << reset.read() << std::endl;
			wait();
		}
	}

	SC_CTOR(monitor) {
		SC_CTHREAD(print, clk.pos());	
	}
};


