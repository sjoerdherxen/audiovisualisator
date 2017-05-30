#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "src/display.h"
#include "src/Audio.h"

#define SAMPLE_RATE 48000
#define SAMPLE_SIZE 256


// Definition of Task Stacks
//##########################
#define TASK_STACKSIZE 2048

OS_STK audioStack[TASK_STACKSIZE];
OS_STK videoStack[TASK_STACKSIZE];


// Definition of Task Priorities
//##############################
#define AUDIO_TASK_PRIORITY      1
#define VIDEO_TASK_PRIORITY      2

void *QueueStack[5];
OS_EVENT *Queue;

// Sample audio, perform FFT and return amplitudes
void audioTask(void* pdata) {

	AudioInit(SAMPLE_RATE, SAMPLE_SIZE);
	FftInit();
	while (1)
	{
		AudioSample();
		
		// Put the returned pointer from DoFft() in a queue so that videoTask can use it.
		OSQPost(Queue, DoFft());

		OSTimeDlyHMSM(0, 0, 0, 75);
	}
}

// Display the appropriate squares on the screen
void videoTask(void* pdata) {

	DisplayInit();

	int i;
	INT8U *err =0;
	int values[10];
	float scaler[11];
	int scale[10];
	int nyquistSize;
	int freqstep;
	
	// zet de verschaling van kolommen op het scherm op logoritmisch.
	scaler[10] = SAMPLE_SIZE;
	scale[9] = SAMPLE_SIZE;
	for(i = 9; i >= 0; i--)
	{
		scaler[i] = scaler[i+1] / 1.5;
		scale[i] = scaler[i];
	}
	nyquistSize = SAMPLE_SIZE / 2;
	freqstep = SAMPLE_RATE / SAMPLE_SIZE;
	
	while (1) {
		float* volume = (float*)OSQPend(Queue, 0, &err);

		// Initialize values array to 0
		for(i=0; i<10; i++)
		{
			values[i] = 0;
		}

		int index = 0;
		int max =0;
		for(i=0; i < nyquistSize; i++)
		{
			// zoek de hoogste waarde in de huidige kolom
			if((int)(log(i+1)*freqstep)/100 > index)
			{
				max=0;
				index++;
			}
			if((int)(volume[i]) > max )
			{
				values[(int)(log(i+1)*freqstep)/91] = (int)volume[i];
				max = (int)volume[i];
			}
//			if(i >= scale[index])
//			{
//				index++;
//			}
//			if((int)volume[i]> values[index])
//			{
//				values[index] = (int)volume[i];
//			}
		}

		// zet de nieuwe waardes op het scherm
		DisplayValues(values);

		OSTimeDlyHMSM(0, 0, 0, 25);
	}
}

int main(void) {

	// Create message queue for communication between the audiotask en videotask
	Queue = OSQCreate(&QueueStack[0], 5);


	// Create the individual tasks
	OSTaskCreateExt(audioTask,
			NULL, (void *) &audioStack[TASK_STACKSIZE - 1],
			AUDIO_TASK_PRIORITY,
			AUDIO_TASK_PRIORITY, audioStack,
			TASK_STACKSIZE,
			NULL, 0);

	OSTaskCreateExt(videoTask,
			NULL, (void *) &videoStack[TASK_STACKSIZE - 1],
			VIDEO_TASK_PRIORITY,
			VIDEO_TASK_PRIORITY, videoStack,
			TASK_STACKSIZE,
			NULL, 0);

	// Start multitasking
	OSStart();
	return 0;
}
