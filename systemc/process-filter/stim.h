#include "systemc.h"
#include <sysc/datatypes/fx/sc_fixed.h>

SC_MODULE(stim)
{
  sc_out<sc_dt::sc_fixed <12, 2, sc_dt::SC_TRN, sc_dt::SC_WRAP> > x;
  sc_out<bool> reset;
  sc_in_clk clock;

  void stimgen() {
    reset.write(true);
    x.write(0);
    wait();
    reset.write(false);
    x.write(1);
    wait();
    x.write(0);
    wait();
  }

  SC_CTOR(stim) {
    SC_CTHREAD(stimgen, clock.pos());
  }

};
