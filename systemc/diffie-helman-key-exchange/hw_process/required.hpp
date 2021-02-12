#include "systemc.h"
#include "digit.h"

#include "components/right_shift.hpp"
#include "components/left_shift.hpp"
#include "components/expand.hpp"
#include "components/and2.hpp"
#include "components/mult2.hpp"
#include "components/mux2.hpp"
#include "components/sub2.hpp"
#include "components/is_gt.hpp"

SC_MODULE(required_part) {
	// in/out signals
	sc_in <NN_DIGIT> t0, t1, c;
	sc_in <NN_HALF_DIGIT> a_low;
	sc_out <NN_DIGIT> t0_new, t1_new;

	// components
	// expander, promotes the integer
	expander exp_1;
	right_shift <NN_DIGIT, NN_HALF_DIGIT_BITS> rshift_1, rshift_2;
	left_shift <NN_DIGIT, NN_HALF_DIGIT_BITS> lshift_1;
	and2 <NN_DIGIT> and2_1, and2_2, and2_3;
	mult2 <NN_DIGIT> mult2_1, mult2_2;
	mux2 <NN_DIGIT> mux2_1, mux2_2;
	sub2 <NN_DIGIT> sub2_1, sub2_2, sub2_3, sub2_4, sub2_5, sub2_6, sub2_7;
	gt <NN_DIGIT> gt_1, gt_2;

	// Sigals holding the constant value
	sc_signal <NN_DIGIT> max_digit_sig, max_half_digit_sig, one;

	// Signal connectors
	sc_signal <NN_DIGIT> c_rshift, c_low, c_high, a_low_digit,
		v, u, t1_sub1, t1_sub2, t1_2, t1_3, t1_final, v_hh, v_rshift,
		t0_sub1, max_subu, v_high, max_sub_vhigh, t0_final;
	
	sc_signal <bool> cmp_1, cmp_2;

	SC_CTOR(required_part): rshift_1("rshift_1"), rshift_2("rshift_2"),
		lshift_1("lshift_1"), and2_1("and2_1"), and2_2("and2_2"), and2_3("and2_3"),
		mult2_1("mult2_1"), mult2_2("mult2_2"), mux2_1("mux2_1"), mux2_2("mux2_2"),
		sub2_1("sub2_1"), sub2_2("sub2_2"), sub2_3("sub2_3"), sub2_4("sub2_4"),
		sub2_5("sub2_5"), sub2_6("sub2_6"), sub2_7("sub2_7"), gt_1("gt_1"), gt_2("gt_2"),
		exp_1("exp_1") {

		// initialize the constant signals signals
		max_half_digit_sig.write(MAX_NN_HALF_DIGIT);
		max_digit_sig.write(MAX_NN_DIGIT);
		one.write(1);

		// connect shifters
		rshift_1.in(c); rshift_1.out(c_rshift);
		rshift_2.in(v); rshift_2.out(v_rshift);
		lshift_1.in(v); lshift_1.out(v_high);

		gt_1.in1(t0_sub1); gt_1.in2(max_subu); gt_1.out(cmp_1);
		gt_2.in1(t0_final); gt_2.in2(max_sub_vhigh); gt_2.out(cmp_2);

		// connect the multiplexers
		mux2_1.in1(t1_sub1); mux2_1.in2(t1); mux2_1.select(cmp_1); mux2_1.out(t1_2);
		mux2_2.in1(t1_sub2); mux2_2.in2(t1_2); mux2_2.select(cmp_2); mux2_2.out(t1_3);

		// connect ands
		and2_1.in1(c_rshift); 	and2_1.in2(max_half_digit_sig); 	and2_1.out(c_high);
		and2_2.in1(c); 			and2_2.in2(max_half_digit_sig); 	and2_2.out(c_low);
		and2_3.in1(v_rshift); 	and2_3.in2(max_half_digit_sig); 	and2_3.out(v_hh);

		// connect multipliers
		mult2_1.in1(c_high); mult2_1.in2(a_low_digit); mult2_1.out(v);
		mult2_2.in1(c_low); mult2_2.in2(a_low_digit); mult2_2.out(u);

		// connect all subtractor components
		// column 1 - column2 = column 3
		sub2_1.in1(t0); 			sub2_1.in2(u); 		sub2_1.out(t0_sub1);
		sub2_2.in1(max_digit_sig); 	sub2_2.in2(u); 		sub2_2.out(max_subu);
		sub2_3.in1(t0_sub1); 		sub2_3.in2(v_high); sub2_3.out(t0_final);
		sub2_4.in1(max_digit_sig); 	sub2_4.in2(v_high); sub2_4.out(max_sub_vhigh);
		sub2_5.in1(t1); 			sub2_5.in2(one); 	sub2_5.out(t1_sub1);
		sub2_6.in1(t1_2); 			sub2_6.in2(one); 	sub2_6.out(t1_sub2);
		sub2_7.in1(t1_3); 			sub2_7.in2(v_hh); 	sub2_7.out(t1_final);

		// connect the expander
		exp_1.in(a_low); exp_1.out(a_low_digit);
	}
};
