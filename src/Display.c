#include "Display.h"

int *prevValues;
alt_u16 *colors;

void DisplayInit(){
	colors = malloc(sizeof(alt_u16) * 10);
	prevValues = malloc(sizeof(int) * 10);

	int i;
	for(i=0;i<10;i++){
		prevValues[i] = 0;
	}

    volatile alt_u16 *pixelbuffer = 0x08000000;

    for(i=0;i<320*480;i++){
    	pixelbuffer[i] = 0xFFFF;
    }

    colors[0] = 0b1111100000000000;
    colors[1] = 0b1111101110000000;
    colors[2] = 0b1111111111100000;
    colors[3] = 0b0001111111100000;
    colors[4] = 0b0000001111100000;
    colors[5] = 0b0000011000011111;
    colors[6] = 0b0000001010011111;
    colors[7] = 0b0000000000111111;
    colors[8] = 0b0111100000011111;
    colors[9] = 0b1111100000001111;
}

void DisplayValues(int* values)
{
	int i;
	for(i = 0; i < 10; i++){
		int y;
		if(prevValues[i] > values[i]){
			for(y = values[i]; y <= prevValues[i]; y++){
				DisplayBlock(i*32 + 2, (7-y)*32 - 18, 28, 28, 0xFFFF);
			}
		} else if(prevValues[i] < values[i]){
			for(y = prevValues[i]; y < values[i]; y++){
				DisplayBlock(i*32 + 2, (7-y)*32 - 18, 28, 28, colors[i]);
			}
		}
		prevValues[i] = values[i];
	}
}

void DisplayBlock(int x, int y, int width, int height, alt_u16 color){
	volatile alt_u16 *pixelbuffer = 0x08000000;
	int x1;
	int y1;
	height = height * 512;
	y = y * 512;
	for(x1=x;x1<(width + x); x1++){
		for(y1 = y; y1 < (height + y); y1 += 512){
			pixelbuffer[x1 + y1] = color;
		}
	}
}
