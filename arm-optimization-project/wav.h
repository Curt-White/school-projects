#ifndef WAV_H
#define WAV_H

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

typedef struct wav_header {
	unsigned char riff[4];
	uint32_t file_size_bytes;
	unsigned char file_header[4];
	unsigned char fmt_chunk_marker[4];
	uint32_t format_length;
	uint16_t format_type;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t bytes_by_capture;
	uint16_t bits_per_sample;
	unsigned char data_chunk_header[4];
	uint32_t data_size;
} wav_header;

typedef struct wav_file_info {
	uint64_t num_samples;
	uint64_t sample_size;
} wav_file_info;

#define wav_file(T)					\
	struct wav_file_##T {			\
		wav_header header;			\
		T *sample_data;				\
	}								\

typedef wav_file (uint16_t) wav_file_16;
typedef wav_file (uint32_t) wav_file_32;

#define destroy_wav_file(file) free(file.sample_data)

// store the value read from file in little endian format to 32 bit integer
void store_little_4(FILE *f, uint32_t *loc) {
	unsigned char buffer4[4];
	fread(buffer4, sizeof(buffer4), 1, f);
	*loc = buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
}

// store the value to the file in little endian format from 16 bit int
void write_little_4(FILE *f, uint32_t value) {
	unsigned char buffer4[4];
	buffer4[0] = (value & 0x000000FF);
	buffer4[1] = (value & 0x0000FF00) >> 8;
	buffer4[2] = (value & 0x00FF0000) >> 16;
	buffer4[3] = (value & 0xFF000000) >> 24;
	fwrite(buffer4, sizeof(buffer4), 1, f);
}

// store the value read from file in little endian format to 16 bit integer
void store_little_2(FILE *f, uint16_t *loc) {
	unsigned char buffer2[2];
	fread(buffer2, sizeof(buffer2), 1, f);
	*loc = buffer2[0] | (buffer2[1] << 8);
}

// store the value to the file in little endian format from 16 bit int
void write_little_2(FILE *f, uint16_t value) {
	unsigned char buffer2[2];
	buffer2[0] = (value & 0x00FF);
	buffer2[1] = (value & 0xFF00) >> 8;
	fwrite(buffer2, sizeof(buffer2), 1, f);
}

// read in the wav header from a file
void read_wav_header(FILE *f, wav_header *header) {
	unsigned char buffer4[4];
	unsigned char buffer2[2];

	fread(header->riff, sizeof(header->riff), 1, f);
	store_little_4(f, &header->file_size_bytes);

	fread(header->file_header, sizeof(header->file_header), 1, f);
	fread(header->fmt_chunk_marker, sizeof(header->fmt_chunk_marker), 1, f);

	store_little_4(f, &header->format_length);
	store_little_2(f, &header->format_type);
	store_little_2(f, &header->channels);
	store_little_4(f, &header->sample_rate);
	store_little_4(f, &header->byte_rate);
	store_little_2(f, &header->bytes_by_capture);
	store_little_2(f, &header->bits_per_sample);
	
	fread(header->data_chunk_header, sizeof(header->data_chunk_header), 1, f);
	store_little_4(f, &header->data_size);
}

// read a wav file into the 
int read_wav(const char *file_name, wav_file_16 *wav, wav_file_info *info) {
	FILE* f = fopen(file_name, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open provided file\n");
		return -1;
	}

	read_wav_header(f, &wav->header);
	if (wav->header.format_type != 1) {
		fprintf(stderr, "Only support pulse code modulated type\n");
		return -1;
	}

	long num_samples = (8 * wav->header.data_size) / (wav->header.channels * wav->header.bits_per_sample);
	long sample_size = (wav->header.channels * wav->header.bits_per_sample) / 8;
	info->num_samples = num_samples;
	info->sample_size = sample_size;

	char data_buffer[sample_size];
	long bytes_in_each_channel = (sample_size / wav->header.channels);
	if ((bytes_in_each_channel  * wav->header.channels) != sample_size) {
		fprintf(stderr, "Incorrect file size\n");
		return -1;
	}

	wav->sample_data = (uint16_t *) calloc(num_samples, sample_size);
	for (int i = 0; i <= num_samples; i++) {
		store_little_2(f, &wav->sample_data[i]);
	}

	fclose(f);
	return 0;
}

// write the wav header to the file which is passed in as f
void write_header(FILE *f, wav_header *header) {
	fwrite(header->riff, sizeof(header->riff), 1, f);
	write_little_4(f, header->file_size_bytes);

	fwrite(header->file_header, sizeof(header->file_header), 1, f);
	fwrite(header->fmt_chunk_marker, sizeof(header->fmt_chunk_marker), 1, f);

	write_little_4(f, header->format_length);
	write_little_2(f, header->format_type);
	write_little_2(f, header->channels);
	write_little_4(f, header->sample_rate);
	write_little_4(f, header->byte_rate);
	write_little_2(f, header->bytes_by_capture);
	write_little_2(f, header->bits_per_sample);
	
	fwrite(header->data_chunk_header, sizeof(header->data_chunk_header), 1, f);
	write_little_4(f, header->data_size);
}

// write data back to a wav file with the name provided, if the file exists it will
// be overwritten. if not, one will be created.
int write_wav(const char *file_name, wav_file_16 *wav, int num_samples) {
	FILE *f = fopen("mu_expand_output.wav", "w+");
	if (f == NULL) {
		fprintf(stderr, "Unable to open/create file with provided name\n");
		return -1;
	}

	write_header(f, &wav->header);
	for (int i = 0; i <= num_samples; i++) {
		write_little_2(f, wav->sample_data[i]);
	}

	fclose(f);
	return 0;
}

#endif
