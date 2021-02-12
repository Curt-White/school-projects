#include "systemc.h"
#include "digit.h"

enum control_state {
	WAIT,
	INPUT,
	EXECUTE_REQUIRED,
	EXECUTE_BONUS,
	OUTPUT,
	FINISH
};

SC_MODULE(controller) {
	sc_in <bool> hw_enable, bonus_done; 
  	sc_out <bool> hw_done, load_input, load_output, ready;
	sc_in_clk clk;

	void loop() {
		control_state s = WAIT;
		hw_done.write(false);
		load_input.write(false);
		load_output.write(false);
		ready.write(false);
		wait();

		while (1) {
			switch (s) {
				case (WAIT): // wait for hardware to be enabled by software
					hw_done.write(false);
					if (hw_enable.read()) {
						s = INPUT;
					}
					break;
				case (INPUT): // load new values into register
					load_input.write(true);
					s = EXECUTE_REQUIRED;
					break;
				case (EXECUTE_REQUIRED): // exeucte the required part and then signal bonus to start
					load_input.write(false);
					ready.write(true);
					s = EXECUTE_BONUS;
					break;
				case (EXECUTE_BONUS): // loop until bonus is done
					if (bonus_done.read()) {
						s = OUTPUT;
					}

					break;
				case (OUTPUT): // output the final values
					load_output.write(true);
					s = FINISH;
					break;
				case (FINISH): // cleanup and wait for hw to be diasbled by software
					hw_done.write(true);
					load_output.write(false);
					ready.write(false);

					if (hw_enable.read() == false) {
						s = WAIT;
					}
					break;
			}

			wait();
		}
	}

	SC_CTOR(controller) {
		SC_CTHREAD(loop, clk.pos());
	}
};
