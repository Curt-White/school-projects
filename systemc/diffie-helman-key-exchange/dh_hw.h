#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"

#include "hw_process/controller.hpp"
#include "hw_process/bonus.hpp"
#include "hw_process/required.hpp"
#include "hw_process/register.hpp"

SC_MODULE (dh_hw) {
	sc_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
	sc_in <NN_HALF_DIGIT> from_sw3;
	sc_out <NN_HALF_DIGIT> to_sw;
	sc_in_clk clk;

	sc_in <bool> hw_enable; 
	sc_out <bool> hw_done;

	sc_signal <bool> load_output, load_input, ready, bonus_done;
	sc_signal <NN_DIGIT> reg0_out, reg1_out, reg2_out, t0_new_out, t1_new_out;
	sc_signal <NN_HALF_DIGIT> reg3_out, b_part_out;

	controller state_controller;
	reg <NN_DIGIT> reg0_in, reg1_in, reg2_in;
	reg <NN_HALF_DIGIT> reg3_in, reg_out;
	bonus_part b_part;
	required_part r_part;

	SC_CTOR (dh_hw): state_controller("state_controller"), b_part("b_part"),
		r_part("r_part"), reg0_in("reg0_in"), reg1_in("reg1_in"), reg2_in("reg2_in"),
		reg3_in("reg3_in"), reg_out("reg_out") {

		// connect the state controller
		state_controller.clk(clk);
		state_controller.hw_enable(hw_enable);
		state_controller.hw_done(hw_done);
		state_controller.load_input(load_input);
		state_controller.load_output(load_output);
		state_controller.ready(ready);
		state_controller.bonus_done(bonus_done);

		// connect registers
		reg0_in.clk(clk);
		reg0_in.in(from_sw0);
		reg0_in.out(reg0_out);
		reg0_in.load(load_input);

		reg1_in.clk(clk);
		reg1_in.in(from_sw1);
		reg1_in.out(reg1_out);
		reg1_in.load(load_input);

		reg2_in.clk(clk);
		reg2_in.in(from_sw2);
		reg2_in.out(reg2_out);
		reg2_in.load(load_input);

		reg3_in.clk(clk);
		reg3_in.in(from_sw3);
		reg3_in.out(reg3_out);
		reg3_in.load(load_input);

		reg_out.clk(clk);
		reg_out.in(b_part_out);
		reg_out.out(to_sw);
		reg_out.load(load_output);

		// connect required part
		r_part.t0(reg0_out);
		r_part.t1(reg1_out);
		r_part.c(reg2_out);
		r_part.a_low(reg3_out);
		r_part.t0_new(t0_new_out);
		r_part.t1_new(t1_new_out);

		// connect bonus part
		b_part.t0_new(t0_new_out);
		b_part.t1_new(t1_new_out);
		b_part.a_low(reg3_out);
		b_part.c(reg2_out);
		b_part.ready(ready);
		b_part.a_low_new(b_part_out);
		b_part.bonus_done(bonus_done);
		b_part.clk(clk);
	}
};

#endif /* end _DH_HW_H_ */
