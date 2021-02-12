#include "systemc.h"
#include <sysc/datatypes/fx/sc_fixed.h>
#include <iomanip>

using namespace std;

SC_MODULE(mon)
{
  sc_in<sc_dt::sc_fixed <12, 2, sc_dt::SC_TRN, sc_dt::SC_WRAP> > x, y;
  sc_in_clk clock;

  void monitor() {
    cout << setw(8) << "Time";
    cout << setw(16) << "x" << setw(16) << "y" << endl;
	
    while (true) {
	cout << setw(8) << sc_time_stamp();
	cout << setw(16) << x.read() << setw(16) << y.read() << endl;
	wait();		
    }
  }

  SC_CTOR(mon) {
    SC_CTHREAD(monitor, clock.pos());
  }

};
