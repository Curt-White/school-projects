#include "../test.h"
#define STEP(value__) (value__ & 0b00001111)
#define BIAS 33

/**
 * A corrected versionof the standard LUT version which ws incorrectly ordered
 * this version is considerably slower than the other version due to the reordering
 * but this version is now correct.
 */

typedef struct exp {
	uint16_t mag;
	unsigned short shift : 4;
} exp;

exp lut[8] = {
	{ 0b00000000100001, 1 },
	{ 0b00000001000010, 2 },
	{ 0b00000010000100, 3 },
	{ 0b00000100001000, 4 },
	{ 0b00001000010000, 5 },
	{ 0b00010000100000, 6 },
	{ 0b00100001000000, 7 },
	{ 0b01000010000000, 8 }
};

/**
 * Pipeline Stages
 * 0. sample = in_buffer[i]
 * 1. 	sample ~= sample
 * 2. 		chord_sign = sample >> 4
 * 3. 		final = lut[chord_sign]
 * 4. 		chord = cho
 * 5. 		shift_amountrd_sign & 0b00000111 = chord + 3
 * 6. 		step = sample & 0b00001111
 * 7. 			rhs = step << shift_amount
 * 8.			ord = final | rhs
 * 9. 				out = ord - BIAS
 */ 

#define STAGE_1 stage1_result = in_buffer[i]; ++i
#define STAGE_2 stage2_result = stage1_result ^ 0b01111111

				// force space in between the first and last instruction in this stage
				// multiple no dependent items in a stage helps reduce bloat
#define STAGE_3 stage3_result = ((uint8_t)(stage2_result >> 4) & 0b00000111); \
				stage3_result2 = stage2_result & 0b00001111; \
				stage4_in = lut[stage3_result]; \
				stage3_result = stage2_result & 0b10000000;

#define STAGE_4 stage4_result.mag = (stage3_result2 << stage4_in.shift); \
				stage4_forward = stage3_result << 8; \
				stage4_result.mag |= stage4_in.mag;\
				stage4_result.mag -= BIAS;

#define	STAGE_5 *out_buffer = (stage4_result.mag << 2) | stage4_forward; ++out_buffer

#define STAGE_COUNT 5

void expand_file(uint8_t *in_buffer, uint16_t *out_buffer, long num_samples) {
	register int i = 0;
	register uint16_t stage3_result, stage4_forward;
	register exp stage4_in, stage4_result;
	register uint8_t stage1_result, stage2_result, stage3_result2;

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
