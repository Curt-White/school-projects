#include "../test.h"
#define STEP(value__) (value__ & 0b00001111)
#define BIAS 33

/**
 *  The same structure as the expand_orig.c except that the if conditionals
 *  have been converted to switch statements. Many further optimizations
 *  were made to this version as well.
 */ 

uint16_t expand(register uint8_t sample) {
	sample = ~sample;
	register uint16_t out = 0;

	register uint8_t chord = (sample & 0b01110000) >> 4;
	switch (chord) {
	case 0:
		out =  32 | (STEP(sample) << 1) | 1; break;
	case 1:
		out =  64 | (STEP(sample) << 2) | 2; break;
	case 2:
		out =  128 | (STEP(sample) << 3) | 4; break;
	case 3:
		out =  256 | (STEP(sample) << 4) | 8; break;
	case 4:
		out =  512 | (STEP(sample) << 5) | 16; break;
	case 5:
		out =  1024 | (STEP(sample) << 6) | 32; break;
	case 6:
		out =  2048 | (STEP(sample) << 7) | 64; break;
	case 7:
		out =  4096 | (STEP(sample) << 8) | 128; break;
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
