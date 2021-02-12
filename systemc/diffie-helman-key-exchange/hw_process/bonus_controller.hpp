#include "systemc.h"
#include "digit.h"

enum bonus_control_state {
	BONUS_WAIT,
	BONUS_LOAD,
	BONUS_EXECUTE,
	BONUS_LOOP,
	BONUS_LOOP_EXIT,
	BONUS_DONE
};

SC_MODULE(bonus_controller) {
	sc_in <bool> loop_cond;
	sc_out<bool> load_tx_reg, load_a_reg, mux_sel;
	sc_in_clk clk;

	// to/from controller
	sc_in <bool> ready;
	sc_out<bool> bonus_done;

	void loop() {
		bonus_control_state s = BONUS_WAIT;
		// initialize all of the output values
		bonus_done.write(false);
		load_tx_reg.write(false);
		load_a_reg.write(false);
		mux_sel.write(false);
		wait();
	
		while (1) {
			switch(s) {
				case BONUS_WAIT: // wait for the ready signal from required controller
					bonus_done.write(false);
					mux_sel.write(false);
					load_a_reg.write(true);
					if (ready.read()) {
						s = BONUS_LOAD;
					}
					break;
				case BONUS_LOAD: // load new values into the registers
					load_tx_reg.write(true);
					s = BONUS_EXECUTE;
					break;
				case BONUS_EXECUTE: // being to execute by close register and switch mux to loop
					load_tx_reg.write(false);
					load_a_reg.write(false);
					mux_sel.write(true);

					if (loop_cond.read()) {
						s = BONUS_LOOP;
					} else {
						s = BONUS_LOOP_EXIT;
					}

					break;
				case BONUS_LOOP: // loop compute until loop condition false
					load_tx_reg.write(true);
					load_a_reg.write(true);
					if (!loop_cond.read()) {
						s = BONUS_LOOP_EXIT;
					} 

					break;
				case BONUS_LOOP_EXIT: //stop loading the register so hold final value
					load_tx_reg.write(false);
					load_a_reg.write(false);
					s = BONUS_DONE;
					break;
				case BONUS_DONE: // notify done so req controller can continue
					bonus_done.write(true);
					if (!ready.read()) {
						s = BONUS_WAIT;
					}
					break;
			}

			wait();
		}
	}

	SC_CTOR(bonus_controller) {
		SC_CTHREAD(loop, clk.pos());
	}
};
