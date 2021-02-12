// #include "../test.h"
#define STEP(value__) (value__ & 0b00001111)
#define BIAS 33
#include "inttypes.h"

void expand_file(uint8_t *in_buffer, uint16_t *out_buffer, long num_samples) {
	register int i = 0;
	register uint8_t next = in_buffer[i++];
	for (i; i < num_samples; i++) {
		asm("mulaw %[result], %[codeword]" : [result]"=r"(out_buffer[i]) : [codeword]"r"(next));
		next = in_buffer[i + 1];
	}
}

int main() {
	return 0;
}
