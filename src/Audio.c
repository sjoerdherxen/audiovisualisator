#include "Audio.h"


alt_up_audio_dev *audio_device;				// Audio device
kiss_fft_cfg mycfg;							// FFT config
unsigned int fifospace;

unsigned int srate;
unsigned int ssize;
double frequency_step;

kiss_fft_scalar *inr;
kiss_fft_cpx *outr;
float *amplitudes;

size_t buffer_index;
size_t i;


int AudioInit(unsigned int sample_rate, unsigned int sample_size){
	
	//printf("AudioInit\n");
	//Open the Altera Audio
	audio_device = alt_up_audio_open_dev("/dev/Audio_Subsystem_Audio");

	if (audio_device == NULL)
	{
		printf("Error: could not open Audio device\n");
		return -1;
	} else
		printf("Successful: Opened Audio device\n");
	
	srate = sample_rate;
	ssize = sample_size;				// Must be a power of 2
	frequency_step = sample_rate / sample_size;

}

int FftInit(){
	//printf("FftInit\n");
	//Initialize a FFT (or IFFT) algorithm's cfg/state buffer.
	mycfg = kiss_fftr_alloc(ssize, 0, NULL, NULL);

	if (mycfg == NULL)
	{
		printf("Error: could not initialize KISS Fast Fourier Transform\n");
		return -1;
	} else
		printf("Successful: Initialized KISS Fast Fourier Transform\n");
	
	// Buffer for the calculated amplitudes.
	amplitudes = calloc(ssize / 2 + 1, sizeof(float));

	//kiss_fft_scalar inr[ssize];			//Inputbuffer for Real FFT
	//kiss_fft_cpx outr[ssize / 2 + 1];		//Outputbuffer for Real FFT

	inr = (float*)malloc(sizeof(kiss_fft_scalar) * ssize);
	outr = (float*)malloc(sizeof(kiss_fft_cpx) * (ssize / 2 + 1));

}

void AudioSample(){
	
	//printf("AudioSample\n");
	volatile int *audio_ptr = (int*) 0x10003040;
	buffer_index = 0;
	
	while (buffer_index < ssize)
	{
		//Read the audio port fifospace register
		fifospace = *(audio_ptr + 1);

		// Check RARC, for > 75% full
		if ((fifospace & 0x000000FF)> 96)
		{
			// Store data until the audio-in FIFO is empty or the memory buffer is full
			while ((fifospace & 0x000000FF) && (buffer_index < ssize))
			{
				// Read right audio buffer, assuming it is practically the same as the left
				inr[buffer_index] = *(audio_ptr + 3);
				++buffer_index;

				// Read the audio port fifospace register
				fifospace = *(audio_ptr + 1);
			}

		}
	}	
}

float* DoFft(){
	
	//printf("DoFft\n");
	kiss_fftr(mycfg, inr, outr);
	
	for(i=0; i<ssize;i++){

		// Using only half the buffer since it is a mirror of the other half
		// This has to do with Nyquist theorem.
		if (i < ssize / 2 + 1)
		{
			amplitudes[i] = sqrt((outr[i].r * outr[i].r) + (outr[i].i * outr[i].i)) / pow(2,32);
		}
	}
	
	return amplitudes;
}




