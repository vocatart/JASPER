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
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "Cannot read file!!!\n");
		return NULL;
	}

	// Check wav file headers
	size_t read_result = fread(header_buf, sizeof(char), 4, file); // RIFF Header
	assert(read_result == 4);
	if (0 != strcmp(header_buf, "RIFF"))
	{
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "RIFF Header Missing!!!\n");
		return NULL;
	}

	fseek(file, 4, SEEK_CUR); // Ignore size of file
	read_result = fread(header_buf, sizeof(char), 4, file); // WAVE Header
	assert(read_result == 4);
	if (0 != strcmp(header_buf, "WAVE"))
	{
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "WAVE Header Missing!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 4, file); // fmt Header
	assert(read_result == 4);
	if (0 != strcat(header_buf, "fmt "))
	{
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "fmt Header Missing!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 4, file); // fmt Header Length
	assert(read_result == 4);
	if (!(16 == header_buf[0] && 0 == header_buf[1] && 0 == header_buf[2] && 0 == header_buf[3]))
	{
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "Incorrect or Missing fmt length!!!\n");
		return NULL;
	}

	read_result = fread(header_buf, sizeof(char), 2, file); // Format ID
	assert(read_result == 2);
	if (!(1 == header_buf[0] && 0 == header_buf[1]))
	{
		fclose(file);
		system("Color 0B");
		fprintf(stderr, "Error: ");
		system("Color 07");
		fprintf(stderr, "Missing format ID!!!\n");
	}

	// Check for more than one channel
	// JASPER will only use the first channel
	read_result = fread(integer_buf, sizeof(char), 2, file);
	assert(read_result == 2);
	num_channels = integer_buf[0];
	if (num_channels > 1)
	{
		system("Color 06");
		fprintf(stderr, "Warning: ");
		system("Color 07");
		fprintf(stderr, " Multiple channels detected. JASPER will only use the first channel.");
	}

	// Sampling Frequency (Hz)
	read_result = fread(integer_buf, sizeof(char), 4, file);
	assert(read_result == 4);
	*samplerate = 0;
	for (int i = 3; i >= 0; i--)
	{
		*samplerate = (*samplerate * 256) + integer_buf[i];
	}

	// Bits per Sample
	fseek(file, 6, SEEK_CUR); // Skip Bytes per Second and Bytes per Sample
	read_result = fread(integer_buf, sizeof(char), 2, file);
	assert(read_result == 2);
	*bitsPerSample = integer_buf[0];
	bytesPerSample = *bitsPerSample / 8; // Get Bits/Sample
	if (*bitsPerSample % 8 > 0) bytesPerSample++; // Rounding up, if needed

	// Find required Data Header
	int chunkLength;
	read_result = fread(header_buf, sizeof(char), 4, file);
	assert(read_result == 4);
	while (strcmp(header_buf, "data") != 0)
	{
		read_result = fread(&chunkLength, sizeof(char), 4, file);
		assert(read_result == 4);
		fseek(file, chunkLength, SEEK_CUR); // Unimportant Chunk
		read_result = fread(header_buf, sizeof(char), 4, file);
		assert(read_result == 4);
	}

	// # of Samples
	read_result = fread(integer_buf, sizeof(char), 4, file);
	assert(read_result == 4);
	*numSample = 0;
	for (int i = 3; i >= 0; i--)
	{
		*numSample = (*numSample * 256) + integer_buf[i];
	}
	*numSample /= (bytesPerSample * num_channels); // Divide by bytes/sample

	if (*cuttoff_ms < 0) // If cuttoff is negative, add to offset instead of subtracting from the end
	{
		*cuttoff_ms = (*numSample * 1000.0 / *samplerate) - (*offset_ms - *cuttoff_ms);
	}

	int start, end;


}