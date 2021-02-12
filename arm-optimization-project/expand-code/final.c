#include "../test.h"
#define BIAS 33

/**
 * This is the final optimized version of the C code from which the final.S expand_file
 * is based on. This file makes extensive use of software pipelining techniques.
 * 
 * All of the stages are shown in the macros below and the pipeline itself is shown in the
 * expand_file function. This is seperated because this would otherwise result in a large amount
 * of repeated code.
 */ 

#define STAGE_1 stage1_result = in_buffer[i]; ++i
#define STAGE_2 stage2_result = stage1_result ^ 0b01111111

#define STAGE_3 stage3_result_shift = (stage2_result >> 4) & 0b00000111;\
				stage3_result_step = (stage2_result << 1) & 0x1E;\
				stage3_result_sign = stage2_result & 0b10000000;\
				stage3_result_step |= 0b00100001

#define STAGE_4 stage4_result = stage3_result_step << stage3_result_shift; \
				stage4_forward = stage3_result_sign << 8; \
 				stage4_result = stage4_result - BIAS

// This below automatically optimizes to use barrel shifter
#define STAGE_5 *out_buffer = (stage4_result << 2) | stage4_forward; ++out_buffer
#define STAGE_COUNT 5

void expand_file(uint8_t *in_buffer, uint16_t *out_buffer, long num_samples) {
	register int i = 0;
	register uint8_t stage1_result, stage2_result;
	register uint16_t stage3_result_sign, stage3_result_step, stage3_result_shift, 
			stage4_result, stage4_forward;

	// "Warm Up" and "Clean Up" the pipeline if there are enough samples to process
	// found this technique in an old apple developers document.
	if(num_samples >= STAGE_COUNT - 1) {
		STAGE_1;
		STAGE_2; STAGE_1;
		STAGE_3; STAGE_2; STAGE_1;
		STAGE_4; STAGE_3; STAGE_2; STAGE_1;

		while(i < num_samples) {
			STAGE_5;	STAGE_4;	STAGE_3;	STAGE_2;	STAGE_1;
		}

		STAGE_5;	STAGE_4;	STAGE_3;	STAGE_2;
		STAGE_5;	STAGE_4;	STAGE_3;
		STAGE_5;	STAGE_4;
		STAGE_5;
	}

	// If there is less samples than stages we cannot perform the warm up and cool down
	// as there is not enough samples
	while(i < num_samples) {
		STAGE_1;
		STAGE_2;
		STAGE_3;
		STAGE_4;
		STAGE_5;
	}
}

int main() {
	test(expand_file);
	return 0;
}
