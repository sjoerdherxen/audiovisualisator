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

void audioTask(void* pdata) {

	AudioInit(SAMPLE_RATE, SAMPLE_SIZE);
	FftInit();
	while (1)
	{
		printf("## Audiotask\n");
		AudioSample();

		// Put the returned pointer from DoFft() in a queue so that videoTask can use it.
		OSQPost(Queue, DoFft());

		OSTimeDlyHMSM(0, 0, 0, 200);
	}
}

/* Prints "Hello World" and sleeps for three seconds */
void videoTask(void* pdata) {

	DisplayInit();

	int i;
	INT8U *err =0;
	int values[10];


	while (1) {
		printf("## Videotask\n");
		float* volume = (float*)OSQPend(Queue, 0, &err);

		// Initialize values array to 0
		for(i=0; i<10; i++)
		{
			values[i] = 0;
		}

		for(i=0; i < SAMPLE_SIZE; i++)
		{
			if((int)volume[i]> values[i / ((SAMPLE_SIZE / 2) /10)])
			{
				values[i / ((SAMPLE_SIZE / 2) /10)] = (int)volume[i];
			}
		}

		DisplayValues(values);

		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}

int main(void) {

	Queue = OSQCreate(&QueueStack[0], 5);

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

	OSStart();
	return 0;
}
