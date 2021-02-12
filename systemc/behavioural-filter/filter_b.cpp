#include "systemc.h"
#include "filter_b.hpp"

void filter_b::output(void) {
	float p0 = r0_out.read() * 0.24;
	float p1 = r1_out.read() * 0.2;
	float p2 = r2_out.read() * 0.25;

	y.write(p0 + p1 + p2);
}

void filter_b::reg_input(void) {
	float in0 = (r0_out.read() * 0.4) + x.read();
	float in1 = (r2_out.read() * -0.5) + (r1_out.read() * -0.8) + x.read();

	r0_in.write(in0);
	r1_in.write(in1);
}

void filter_b::update(void) {
	// reset
	r0_out.write(0);
	r1_out.write(0);
	r2_out.write(0);
	wait();


	while (1) {
		r0_out.write(r0_in.read());
		r1_out.write(r1_in.read());
		r2_out.write(r1_out.read());
		wait();
	}
}

