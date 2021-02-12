#include "stim.hpp"
#include "memory.hpp"

#define write_mem_cmd(cmd__, data__, addr__) do { \
	data.write(data__); 	\
	addr.write(addr__); 	\
	comm.write(cmd__);		\
	new_comm.write(true); 	\
	} while(0)

#define read_mem_cmd(cmd__, addr__) do { \
	addr.write(addr__); 	\
	comm.write(cmd__);		\
	new_comm.write(true); 	\
	} while(0)

/**
 * after done calling command deassert the new command line
 * and wait until the complete line is deasserted by the mem module.
 * First wait is wait for new comn deassert to propagate
 * Second wait is to wait for complete deassert
 */
#define end_mem_cmd() do {  \
	new_comm.write(false); \
	wait();	\
	wait(); \
	assert(complete.read() == false); \
	} while(0)

void stim::run() {
	// test writing 0s to addr 0 (test lower bound)
	write_mem_cmd(WTBYT, 0, 0);
	while (complete.read() == false) wait();
	end_mem_cmd();
	data.write(Z); // clear the data lines

	// test writing 255 to addr 255 (upper bound testing)
	write_mem_cmd(WTBYT, 255, 255);
	while (complete.read() == false) wait();
	end_mem_cmd();
	data.write(Z);

	// read from first address test
	read_mem_cmd(RDBYT, 0);
	while (complete.read() == false) wait();
	assert(data.read() == 0);
	end_mem_cmd();
	assert(data.read() == Z);

	// read from last address test
	read_mem_cmd(RDBYT, 255);
	while (complete.read() == false) wait();
	assert(data.read() == 255);
	end_mem_cmd();
	assert(data.read() == Z);

	// test writing full block of same values
	write_mem_cmd(WTBLK, 170, 64);
	while (complete.read() == false) wait();
	end_mem_cmd();
	data.write(Z);

	// write a single byte to memory
	write_mem_cmd(WTBYT, 85, 128);
	while (complete.read() == false) wait();
	end_mem_cmd();
	data.write(Z);

	// test reading an enitre block
	read_mem_cmd(RDBLK, 64);
	wait();
	wait();
	for (int i = 0; i < 4; i++) {
		wait();
		assert(data.read() == 170);
	}

	while (complete.read() == false) wait();
	end_mem_cmd();
	assert(data.read() == Z);

	// read a single byte from memory
	read_mem_cmd(RDBYT, 128);
	while (complete.read() == false) wait();
	assert(data.read() == 85);
	end_mem_cmd();
	assert(data.read() == Z);

	// test writing full block of different values
	write_mem_cmd(WTBLK, 252, 64);
	wait(); data.write(253);
	wait(); data.write(254);
	wait(); data.write(255);
	while (complete.read() == false) wait();
	end_mem_cmd();
	data.write(Z);

	// test reading an enitre block
	read_mem_cmd(RDBLK, 64);
	wait();	// couple cycle wait time for a read
	wait();
	wait(); assert(data.read() == 252); 
	wait(); assert(data.read() == 253); 
	wait(); assert(data.read() == 254); 
	wait(); assert(data.read() == 255); 
	while (complete.read() == false) wait();
	end_mem_cmd();
	assert(data.read() == Z);

	// test the reset function
	read_mem_cmd(RDBYT, 128);
	while (complete.read() == false) wait();
	reset.write(true);
	wait(); wait();
	assert(complete.read() == false);

	sc_stop();
}