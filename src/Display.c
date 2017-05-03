#include "Display.h"

void DisplayValues(int* values)
{
	//DisplayBlock(0, 0, 1,5, 0b1111100000000000);
	int i;
	for(i = 0; i < 10; i++){
		int y;
		for(y = 1; y <= 8; y++){
			if(y <= values[i]){
				DisplayBlock(i*32 + 2, (8-y)*32 - 18, 28, 28, 0b1111100000000000);
			} else {
				DisplayBlock(i*32 + 2, (8-y)*32 - 18, 28, 28, 0b1111111111111111);
			}
		}
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
