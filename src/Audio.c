#include "Audio.h"

alt_up_audio_dev *audio_device;				// Audio device
kiss_fft_cfg mycfg;							// FFT config

unsigned int srate;
unsigned int ssize;
double frequency_step;

float *amplitudes;

size_t buffer_index;
size_t i;

void AudioInit(unsigned int sample_rate, unsigned int sample_size){
	
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
	frequency_step = sampling_rate / sample_size;

}

void FftInit(){
	
	//Initialize a FFT (or IFFT) algorithm's cfg/state buffer.
	mycfg = kiss_fftr_alloc(sample_size, 0, NULL, NULL);

	if (mycfg == NULL)
	{
		printf("Error: could not initialize KISS Fast Fourier Transform\n");
		return -1;
	} else
		printf("Successful: Initialized KISS Fast Fourier Transform\n");
	
	// Buffer for the calculated amplitudes.
	amplitudes = calloc(sample_size / 2 + 1, sizeof(float));
	
	kiss_fft_scalar inr[sample_size];			//Inputbuffer for Real FFT
	kiss_fft_cpx outr[sample_size / 2 + 1];		//Outputbuffer for Real FFT
	
}

void AudioSample(){
	
	volatile int *audio_ptr = (int*) 0x10003040;
	buffer_index = 0;
	
	while (buffer_index < sample_size)
	{
		//Read the audio port fifospace register
		fifospace = *(audio_ptr + 1);

		// Check RARC, for > 75% full
		if ((fifospace & 0x000000FF)> 96)
		{
			// Store data until the audio-in FIFO is empty or the memory buffer is full
			while ((fifospace & 0x000000FF) && (buffer_index < sample_size))
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

int DoFft(){
	
	kiss_fftr(mycfg, inr, outr);
	
	for(i=0; i<sample_size;i++){

		// Using only half the buffer since it is a mirror of the other half
		// This has to do with Nyquist theorem.
		if (i < sample_size / 2 + 1)
		{
			amplitudes[i] = sqrt((outr[i].r * outr[i].r) + (outr[i].i * outr[i].i));
			
		}
	}
	
	return *amplitudes;
}




