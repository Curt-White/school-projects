#include "systemc.h"

template <class T> class FIFO_WRITE_HS : public sc_module, public sc_fifo_out_if <T> {
public:
	sc_in_clk clock; sc_out <T> data; sc_out <bool> valid; sc_in <bool> ready;

	void write (const T& x) { // blocking write
		data.write (x); 
		valid.write (true); // drive data line, signal that data is valid
		// Wait until data is read
		do { wait (clock->posedge_event()); } while (ready.read() != true);
		valid.write (false); // make sure data is not read twice
	}
	
	SC_CTOR (FIFO_WRITE_HS) { valid.initialize (false); }
	// Here, provide dummy implementations for unneeded sc_fifo_out_if <T> methods

	int num_free() const {}
	bool nb_write(const T &a) {}
	const sc_event &data_read_event() const {}
};
