#include "systemc.h"
#include <sysc/datatypes/fx/sc_fixed.h>
#include "stim.h"
#include "filter.h"
#include "mon.h"

int sc_main(int argc, char* argv[]) {
  sc_signal <sc_dt::sc_fixed <12, 2, sc_dt::SC_TRN, sc_dt::SC_WRAP> > x, y;
  sc_signal <bool> reset;
  sc_clock clock("TestClock", 10, SC_NS, 0.5, 5, SC_NS); 
  
  stim Stim1("Stimulus");
  Stim1.x(x);
  Stim1.reset(reset);
  Stim1.clock(clock);

    
  filter DUT("filter");
  DUT.x(x);
  DUT.y(y);
  DUT.clock(clock);
  DUT.reset(reset);

  mon Mon1("Monitor");
  Mon1.x(x);
  Mon1.y(y);
  Mon1.clock(clock);
  
  sc_trace_file* Tf = sc_create_vcd_trace_file("filter_traces");
  Tf->set_time_unit(1, SC_NS);
  sc_trace(Tf, x, "x");
  sc_trace(Tf, y, "y");
  sc_trace(Tf, reset, "reset");
  sc_trace(Tf, clock, "clock");

  sc_start(130, SC_NS);
  sc_close_vcd_trace_file(Tf);
  
  return 0;
}
