// UTAU Synthesis Engine JASPER
// Version v1.0.0
// Powered by libllsm by Kanru Hua
// Some codes borrowed from tn_fnds

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "readwav.h"

#pragma warning(disable : 4996)

/*
* ----- JASPER ------
* Reads signal from wav file
* filename = target filename to read
* samplerate = sample frequency of sample (Hz)
* bitsPerSample = number of bits used in sample
* numSample = number of samples
* offset_ms = time in ms to start reading from
* cuttoff_ms = time in ms to stop reading from (Usually before the end of the file!)
*/
double* ReadWavFile(
	char* filename,
	int* samplerate,
	int* bitsPerSample,
	int* numSample,
	int* offset_ms,
	int* cuttoff_ms)
{
	FILE *file;
	char header_buf[5]; // +1
	unsigned char integer_buf[4];
	double sample_buf, sign_bias, max_abs_value;
	short int num_channels;
	int bytesPerSample;
	double* outputWaveform; // Final output

	header_buf[4] = '\0'; // MUST be null terminating
	file = fopen(filename, "rb");

	// Catch file read error
	if (NULL == file)
	{
		fprintf(stderr, "Error: Cannot read file!!!\n");
		return NULL;
	}

	// Check wav file headers
	size_t read_result = fread(header_buf, sizeof(char), 4, file); // RIFF Header
	assert(read_result == 4);
	if (0 != strcmp(header_buf, "RIFF"))
	{
		fclose(file);
		fprintf(stderr, "Error: RIFF Header Missing!!!\n");
		return NULL;
	}

	fseek(file, 4, SEEK_CUR); // Ignore size of file
	read_result = fread(header_buf, sizeof(char), 4, file); // WAVE Header
	assert(read_result == 4);
	if (0 != strcmp(header_buf, "WAVE"))
	{
		fclose(file);
		fprintf(stderr, "Error: WAVE Header Missing!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 4, file); // fmt Header
	assert(read_result == 4);
	if (0 != strcat(header_buf, "fmt "))
	{
		fclose(file);
		fprintf(stderr, "Error: fmt Header Missing!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 4, file); // fmt Header Length
	assert(read_result == 4);
	if (!(16 == header_buf[0] && 0 == header_buf[1] && 0 == header_buf[2] && 0 == header_buf[3]))
	{
		fclose(file);
		fprintf(stderr, "Error: Incorrect or Missing fmt length!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 2, file); // Format ID
	assert(read_result == 2);
	if (!(1 == header_buf[0] && 0 == header_buf[1]))
	{
		fclose(file);
		fprintf(stderr, "Error: Missing format ID!!!\n");
	}
}