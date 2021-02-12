#include "stdint.h"
#include <stdio.h>
#include "wav.h"

#define BIAS 33
#define MULAW_MAX 0x1FFF

uint8_t compress(uint16_t orig) {
	if (orig == 0) return 0;

	// shift the sample down by two to get the 14-sample
	uint8_t sign = ((int16_t) orig) >= 0 ? 0x80 : 0;
	uint16_t mag = ((int16_t) orig) < 0 ? -orig : orig;
	mag >>= 2;

	mag += BIAS;
	if (mag > MULAW_MAX) {
    	mag = MULAW_MAX;
  	}

	uint16_t mask = 0x1000;
	int i = 7;
	while (1) {
		if (mask & mag) {
			break;
		}

		mask >>= 1;
		i--;
	}

	return ~(sign | (i << 4) | ((mag >> (i + 1)) & 0x0F));
}

void compress_file(uint16_t *in_buffer, uint8_t *out_buffer, long num_samples) {
	for (int i = 0; i < num_samples; i++) {
		out_buffer[i] = compress(in_buffer[i]);
	}
}

int main() {
	wav_file_info info;
	wav_file_16 in_wav;
	read_wav("./sample.wav", &in_wav, &info);

	uint8_t *out_buffer = (uint8_t *) calloc(info.num_samples, sizeof(uint8_t));
	if (out_buffer == NULL) {
		printf("Error allocating memory\n");
		return -1;
	}

	compress_file(in_wav.sample_data, out_buffer, info.num_samples);

	FILE *f = fopen("./compressed.txt", "w+");
	fwrite(&info, sizeof(wav_file_info), 1, f);
	fwrite(&in_wav, sizeof(wav_file_16), 1, f);
	fwrite(out_buffer, sizeof(uint8_t), info.num_samples, f);

	destroy_wav_file(in_wav);
	free(out_buffer);
}
