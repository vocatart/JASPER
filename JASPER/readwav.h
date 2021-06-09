// UTAU Synthesis Engine JASPER
// Version v1.0.0
// Powered by libllsm by Kanru Hua

// Read signal from .wav file
double* ReadWavFile(char* filename, int *samplerate, int *bitsPerSample, int *numSample, int *offset_ms, int *cuttoff_ms);