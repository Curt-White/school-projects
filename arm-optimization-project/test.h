#ifndef TEST_H
#define TEST_H

#include "stdint.h"
#include "stdio.h"
#include "wav.h"

// pass in an audio expansion function which takes and input and output list
// will return 0 on success
int test(void (*expander) (uint8_t*, uint16_t*, long)) {
	wav_file_info info;
	wav_file_16 in_wav;

	FILE *f = fopen("compressed.txt", "r");
	if (f == NULL) {
		printf("Could not open compressed.txt, please create one\n");
		return -1;
	}

	fread(&info, sizeof(wav_file_info), 1, f);
	fread(&in_wav, sizeof(wav_file_16), 1, f);

	uint8_t *in_buffer = (uint8_t *) calloc(info.num_samples, sizeof(uint8_t));
	uint16_t *out_buffer = (uint16_t *) calloc(info.num_samples, sizeof(uint16_t));
	uint16_t *out_buffer_cmp = (uint16_t *) calloc(info.num_samples, sizeof(uint16_t));
	if (out_buffer == NULL || in_buffer == NULL || out_buffer_cmp == NULL ) {
		printf("Error allocating memory\n");
		return -1;
	}

	fread(in_buffer, sizeof(uint8_t), info.num_samples, f);
	expander(in_buffer, out_buffer, info.num_samples);

	FILE *f2 = fopen("./expanded.txt", "r");
	if (f2 == NULL) {
		printf("Could not open expanded.txt, please create one\n");
		return -1;
	}

	fread(out_buffer_cmp, sizeof(uint16_t), info.num_samples, f2);
	// compare the output with the expected output
	for (int i = 0; i < info.num_samples; i++) {
		if (out_buffer_cmp[i] != out_buffer[i]) {
			printf("%X %X %X\n",out_buffer_cmp[i], out_buffer[i], in_buffer[i]);
		}
	}

	free(in_buffer);
	free(out_buffer);
	free(out_buffer_cmp);
	
	fclose(f);
	fclose(f2);
	return 0;
}

#endif
