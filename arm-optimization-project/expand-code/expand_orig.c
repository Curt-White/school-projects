#include "../test.h"
#define STEP(value__) (value__ & 0b00001111)
#define BIAS 33

/** 
 *  This was the first implementation of the mu-law expansion algorithm
 *  which only contains a series of conditionals. This file was initially
 *  benchmarked against the expand_shift and expand_switch files.
 */

uint16_t expand(register uint8_t sample) {
	sample = ~sample;
	register uint16_t out = 0;

	register uint8_t chord = (sample & 0b01110000) >> 4;
	if (chord == 0) {
		out =  32 | (STEP(sample) << 1) | 1;
	} else if (chord == 1) {
		out =  64 | (STEP(sample) << 2) | 2;
	} else if (chord == 2) {
		out =  128 | (STEP(sample) << 3) | 4;
	} else if (chord == 3) {
		out =  256 | (STEP(sample) << 4) | 8;
	} else if (chord == 4) {
		out =  512 | (STEP(sample) << 5) | 16;
	} else if (chord == 5) {
		out =  1024 | (STEP(sample) << 6) | 32;
	} else if (chord == 6) {
		out =  2048 | (STEP(sample) << 7) | 64;
	} else if (chord == 7) {
		out =  4096 | (STEP(sample) << 8) | 128;
	}

	out -= BIAS;
	out <<= 2;
	if (!(sample & 0b10000000)) {
		out |= 0b1000000000000000;
	}

	return out;
}

void expand_file(uint8_t *in_buffer, uint16_t *out_buffer, long num_samples) {
	for (int i = 0; i < num_samples; i++) {
		out_buffer[i] = expand(in_buffer[i]);
	}
}

int main() {
	test(expand_file);
	return 0;
}
