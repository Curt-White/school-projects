#include "systemc.h"
#include "digit.h"

#include "components/eq2.hpp"
#include "components/add2.hpp"
#include "components/sub2.hpp"
#include "components/gteq.hpp"
#include "components/logic_and2.hpp"
#include "components/logic_or2.hpp"
#include "components/is_gt.hpp"
#include "components/mux2.hpp"
#include "bonus_controller.hpp"
#include "register.hpp"

SC_MODULE(bonus_part) {
	// input and output signals to bonus part
	sc_in <NN_DIGIT> t0_new, t1_new, c;
	sc_in <NN_HALF_DIGIT> a_low;
	sc_out <NN_HALF_DIGIT> a_low_new;
	sc_in <bool> ready;
	sc_out<bool> bonus_done;
	sc_in_clk clk;

	// internal control signals
	sc_signal <bool> load_tx_reg, load_a_reg, loop_cond, mux_sel;
	
	// internal signals
	sc_signal <NN_DIGIT> t0_mux_out, t1_mux_out, t0_loop_mux_in,
		t1_loop_mux_in, t0_reg_out, t1_reg_out, t0_sub_c, max_sub_c, t1_sub_1;

	sc_signal <bool> cmp_1, cmp_2, cmp_3, cmp_4, cmp_5;	

	sc_signal <NN_HALF_DIGIT> a_low_mux_out, a_low_loop_mux_in;
	
	// signals with constant values
	sc_signal <NN_DIGIT> one, zero, max_digit_sig;
	sc_signal <NN_HALF_DIGIT> one_half;

	// components
	bonus_controller bc;

	reg <NN_HALF_DIGIT> a_low_reg;
	reg <NN_DIGIT> t0_reg, t1_reg;

	mux2 <NN_HALF_DIGIT> a_low_mux;
	mux2 <NN_DIGIT> t0_mux, t1_mux, t1_dec_mux;

	gt <NN_DIGIT> gt_1, gt_2;
	gteq <NN_DIGIT> gteq_1;
	sub2 <NN_DIGIT> sub_1, sub_2, sub_3;
	add2 <NN_HALF_DIGIT> add_1;
	logic_and2 land_1;
	logic_or2 lor_1;
	eq2 <NN_DIGIT> eq_1;

	SC_CTOR(bonus_part): bc("bc"), a_low_reg("a_low_reg"), t0_reg("t0_reg"), t1_reg("t1_reg"),
		a_low_mux("a_low_mux"), t0_mux("t0_mux"), t1_mux("t1_mux"), t1_dec_mux("t1_dec_mux"),
		gt_1("gt_1"), gt_2("gt_2"), gteq_1("gteq_1"), sub_1("sub_1"), sub_2("sub_2"), 
		sub_3("sub_3"), add_1("add_1"), land_1("land_1"), lor_1("lor_1"), eq_1("eq_1") {
		
		// initialize the constant signals
		max_digit_sig.write(MAX_NN_DIGIT);
		one.write(1);
		one_half.write(1);
		zero.write(0);

		// initialize the bonus bonus controller
		bc.clk(clk);
		bc.loop_cond(loop_cond);
		bc.load_tx_reg(load_tx_reg);
		bc.load_a_reg(load_a_reg);
		bc.mux_sel(mux_sel);
		bc.ready(ready);
		bc.bonus_done(bonus_done);

		// registers
		a_low_reg.in(a_low_mux_out); a_low_reg.clk(clk);
		a_low_reg.load(load_a_reg);  a_low_reg.out(a_low_new);

		t0_reg.in(t0_mux_out);		t0_reg.clk(clk);
		t0_reg.load(load_tx_reg);	t0_reg.out(t0_reg_out);

		t1_reg.in(t1_mux_out);		t1_reg.clk(clk);
		t1_reg.load(load_tx_reg);	t1_reg.out(t1_reg_out);

		// comparissons
		gt_1.in1(t1_reg_out);		gt_1.in2(zero);			gt_1.out(cmp_1);
		gt_2.in1(t0_loop_mux_in); 	gt_2.in2(max_sub_c);	gt_2.out(cmp_5);
		eq_1.in1(t1_reg_out);		eq_1.in2(zero);			eq_1.out(cmp_3);
		gteq_1.in1(t0_reg_out);		gteq_1.in2(c);			gteq_1.out(cmp_4);
		land_1.in1(cmp_3);			land_1.in2(cmp_4);		land_1.out(cmp_2);
		lor_1.in1(cmp_1);			lor_1.in2(cmp_2);		lor_1.out(loop_cond);

		// multiplexers
		a_low_mux.in1(a_low_loop_mux_in); 	a_low_mux.in2(a_low); 
		a_low_mux.out(a_low_mux_out); 		a_low_mux.select(mux_sel);

		t0_mux.in1(t0_loop_mux_in);			t0_mux.in2(t0_new);
		t0_mux.out(t0_mux_out);				t0_mux.select(mux_sel);

		t1_mux.in1(t1_loop_mux_in);			t1_mux.in2(t1_new);
		t1_mux.out(t1_mux_out);				t1_mux.select(mux_sel);

		t1_dec_mux.in1(t1_sub_1); 			t1_dec_mux.in2(t1_reg_out);
		t1_dec_mux.out(t1_loop_mux_in);		t1_dec_mux.select(cmp_5);

		// arithmetic units
		sub_1.in1(t0_reg_out);		sub_1.in2(c); 			sub_1.out(t0_loop_mux_in);
		sub_2.in1(max_digit_sig); 	sub_2.in2(c); 			sub_2.out(max_sub_c);
		sub_3.in1(t1_reg_out); 		sub_3.in2(one);			sub_3.out(t1_sub_1);
		add_1.in1(a_low_new);		add_1.in2(one_half);	add_1.out(a_low_loop_mux_in);
	}
};
