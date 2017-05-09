#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "src/display.h"
#include "src/Audio.h"
#include <altera_up_avalon_audio.h>

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK task1_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1

void task2(void* pdata) {
	unsigned int buffer[128];
	alt_up_audio_dev* a;
	a = alt_up_audio_open_dev("/dev/Audio_Subsystem_Audio");
	if (a == NULL) {
		printf("fuck!!\nMe dinge is kapot dat is bale??\nFIX DIE SHIT");
		return;
	}
	alt_up_audio_enable_read_interrupt(a);

	volatile int *audioSettings = 0x10003040;
	*audioSettings = 5;
	*audioSettings = 1;
	//printf("audio%x\n", *audioSettings); // 0011 0000 0100 0000
	while (1) {

		*audioSettings = 5;
		*audioSettings = 1;
		int wordCount = 0;

		//wordCount = alt_up_audio_read_fifo_avail(a, 1);
		while (!(*audioSettings & 1 << 8)) {
			wordCount = alt_up_audio_read_fifo_avail(a, 1);

			//printf("nop%d\n", wordCount);
		}
		//alt_up_audio_record_r(a, buffer, 128-wordCount);

		int i;
		//printf("count: %d\n", 128-wordCount);
		for (i = 0; i < 128-wordCount; i++) {
			if (audioSettings[i + 3])
				printf("%d\n", audioSettings[i + 3]);
		}
		OSTimeDlyHMSM(0, 0, 31, 1);
	}
}

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata) {
	DisplayInit();
	AudioInit();
	while (1) {
		AudioSample();
		int* values;
		values = malloc(sizeof(int) * 10);
		int sample = 2;
		if (sample > 0xff000000) {
			values[1] = 0;
			values[0] = 1;
		} else if (sample < 0xffff) {
			values[1] = 1;
			values[0] = 0;
		} else {
			values[1] = 1;
			values[0] = 1;
		}
		//values[1] = rand()%8;
		values[2] = rand() % 2;
		values[3] = rand() % 3;
		values[4] = rand() % 4;
		values[5] = rand() % 5;
		values[6] = rand() % 6;
		values[7] = rand() % 7;
		values[8] = rand() % 1;
		values[9] = rand() % 1;
		DisplayValues(values);
		free(values);
		OSTimeDlyHMSM(0, 0, 2, 0);
	}
}

int main(void) {

	OSTaskCreateExt(task2,
	NULL, (void *) &task1_stk[TASK_STACKSIZE - 1],
	TASK1_PRIORITY,
	TASK1_PRIORITY, task1_stk,
	TASK_STACKSIZE,
	NULL, 0);

	OSStart();
	return 0;
}
