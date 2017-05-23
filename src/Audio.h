#include <stdio.h>
#include "includes.h"
#include <math.h>
#include "./kiss_fftr.h"
#include "./kiss_fft.h"
#include <altera_up_avalon_audio.h>

int AudioInit(unsigned int sample_rate, unsigned int sample_size);
int FftInit();
void AudioSample();
float* DoFft();

