#include "systemc.h"
#include "multiplier.hpp"
#include "adder.hpp"
#include "register.hpp"

SC_MODULE (filter_s) {
	sc_in <float> x;
	sc_out <float> y;
	
	sc_in_clk clk;
	sc_in <bool> reset;
	
	sc_signal <float> a0, a1, a2, a3, a4, a5, a6;
	sc_signal <float> r0_out, r1_out, r2_out;
	sc_signal <float> m0_out, m1_out, m2_out, m3_out, m4_out, m5_out;
	sc_signal <float> a0_out, a1_out, a2_out, a3_out, a4_out;

	adder adder0, adder1, adder2, adder3, adder4;
	multiplier mult0, mult1, mult2, mult3, mult4, mult5;
	reg reg0, reg1, reg2;

	SC_CTOR(filter_s):
		adder0("adder0"), adder1("adder1"), adder2("adder2"),
		adder3("adder3"), adder4("adder4"),
		mult0("mult0"), mult1("mult1"), mult2("mult2"),
		mult3("mult3"), mult4("mult4"), mult5("mult5"),
		reg0("reg0"), reg1("reg1"), reg2("reg2") {

		adder0.a(x); adder0.b(m0_out); adder0.c(a0_out);
		adder1.a(m1_out); adder1.b(a4_out); adder1.c(y);
		adder2.a(x); adder2.b(a3_out); adder2.c(a2_out);
		adder3.a(m2_out); adder3.b(m4_out); adder3.c(a3_out);
		adder4.a(m3_out); adder4.b(m5_out); adder4.c(a4_out);

		mult0.a(r0_out); mult0.b(a0); mult0.c(m0_out);
		mult1.a(r0_out); mult1.b(a1); mult1.c(m1_out);
		mult2.a(r1_out); mult2.b(a2); mult2.c(m2_out);
		mult3.a(r1_out); mult3.b(a3); mult3.c(m3_out);
		mult4.a(r2_out); mult4.b(a4); mult4.c(m4_out);
		mult5.a(r2_out); mult5.b(a5); mult5.c(m5_out);

		reg0.reset(reset);
		reg0.clk(clk);
		reg0.in(a0_out);
		reg0.out(r0_out);
		
		reg1.reset(reset);
		reg1.clk(clk);
		reg1.in(a2_out);
		reg1.out(r1_out);

		reg2.reset(reset);
		reg2.clk(clk);
		reg2.in(r1_out);
		reg2.out(r2_out);
		
		a0.write(0.4); a1.write(0.24); a2.write(-0.8);
		a3.write(0.2); a4.write(-0.5); a5.write(0.25);
	}
};
