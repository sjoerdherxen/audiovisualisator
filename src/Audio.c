#include "Audio.h"


void AudioInit(){
	volatile int *audio_ptr = (int*) 0x10003040;

	*audio_ptr = 0x4;
	*audio_ptr = 0x0;

}

int AudioSample(){
	volatile int *audio_ptr = (int*) 0x10003040;

	*audio_ptr = 0x4;
	*audio_ptr = 0x0;

	int bufferSize = 2000;
	int *buffer;
	buffer = malloc(sizeof(int) * bufferSize);

	int i;
	for(i=0;i<bufferSize; i++){
		buffer[i] = *(audio_ptr + 2);
	}


	*audio_ptr = 0x8;
	*audio_ptr = 0x0;
	for(i=0;i<bufferSize; i++){
		*(audio_ptr + 2) = buffer[i];
	}
	free(buffer);
	return *(audio_ptr + 2);
}



