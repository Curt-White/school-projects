#include "systemc.h"
#include <sysc/datatypes/fx/sc_fixed.h>
 
using fixed = sc_dt::sc_fixed <12, 2, sc_dt::SC_TRN, sc_dt::SC_WRAP>;

SC_MODULE(filter)
{
  sc_in<fixed> x;
  sc_out<fixed> y;

  sc_in_clk clock;
  sc_in<bool> reset;

  fixed c0, c1, c2, c3, c4, c5;
  sc_signal <fixed> R0out, R1out, R2out;
  sc_signal <fixed> R0in, R1in;
  


  void seq_proc() {  
    while (1) {
      if (reset.read() == true) {
	      R0out.write(0);
	      R1out.write(0);
	      R2out.write(0);
	    }
      else {
 	      R0out.write( R0in.read() );
 	      R1out.write( R1in.read() );
 	      R2out.write( R1out.read() );
	    }
	    wait();
    }
  }

 
  void comb_proc1() {
    fixed R0s = R0out.read();
    fixed R1s = R1out.read();
    fixed R2s = R2out.read();

    y.write( c3*R0s + c4*R1s + c5*R2s );
  }


  void comb_proc2() {
    fixed xs = x.read();
    fixed R0s = R0out.read();
    fixed R1s = R1out.read();
    fixed R2s = R2out.read();

    R0in.write( xs + c0*R0s );
    R1in.write( xs + c1*R1s + c2*R2s );
  }

  
  SC_CTOR(filter) { 
    SC_CTHREAD(seq_proc, clock.pos());
    SC_METHOD(comb_proc1);  sensitive << R0out << R1out << R2out;
    SC_METHOD(comb_proc2);  sensitive << x << R0out << R1out << R2out;

    c0 = 0.4;   c1 = -0.8;  c2 = -0.5;
    c3 = 0.24;  c4 = 0.2;   c5 = 0.25;
  }

};
