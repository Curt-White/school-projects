#include "stdint.h"
#include "stdlib.h"
#include "wav.h"
#include "stdio.h"
#include "test.h"

#define BIAS 33

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
	wav_file_info info;
	wav_file_16 in_wav;

	FILE *f = fopen("compressed.txt", "r");
	fread(&info, sizeof(wav_file_info), 1, f);
	fread(&in_wav, sizeof(wav_file_16), 1, f);

	uint8_t *in_buffer = (uint8_t *) calloc(info.num_samples, sizeof(uint8_t));
	uint16_t *out_buffer = (uint16_t *) calloc(info.num_samples, sizeof(uint16_t));
	if (out_buffer == NULL || in_buffer == NULL) {
		printf("Error allocating memory\n");
		return -1;
	}
	
	fread(in_buffer, sizeof(uint8_t), info.num_samples, f);
	expand_file(in_buffer, out_buffer, info.num_samples);
	in_wav.sample_data = out_buffer;
	in_wav.header.format_type = 0x07;
	
	FILE *f2 = fopen("./expanded.txt", "w+");
	fwrite(out_buffer, sizeof(uint16_t), info.num_samples, f2);

	write_wav("./out.wav", &in_wav, info.num_samples);
	destroy_wav_file(in_wav);
	free(in_buffer);

	fclose(f);
	fclose(f2);
	return 0;
}
