#include "../test.h"
#include "stdint.h"
#define BIAS 33

/**
 *  One of the initial implementations. It is a versy simple implementations
 *  and is not optimized at all. This version eventually led to the software
 *  pipelining solution instead of the original implementation,
 */ 

uint16_t expand(uint8_t sample) {
	sample = ~sample;

	uint8_t shift = ((sample & 0b01110000) >> 4);
	uint16_t out = (sample & 0b00001111) << 1 | 0b00100001;
	out <<= shift;
	out -= BIAS;
	out <<= 2;

	if (!(sample & 0b10000000)) {
		out |= 0b1000000000000000;
	}

	return out;
}

void expand_file(register uint8_t *in_buffer, register uint16_t *out_buffer, long num_samples) {
	register int i;
	for (i = num_samples; i != 0; i--) {
		out_buffer[i] = expand(in_buffer[i]);
	}

	out_buffer[i] = expand(in_buffer[i]);
}

int main() {
	test(expand_file);
	return 0;
}
