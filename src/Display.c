#include "Display.h"

int *prevValues;
alt_u16 *colors;

// Initialiseer Display
void DisplayInit(){
	colors = malloc(sizeof(alt_u16) * 10);
	
	// oude waardes van de hoogtes initialiseren
	prevValues = malloc(sizeof(int) * 10);
	int i;
	for(i=0;i<10;i++){
		prevValues[i] = 0;
	}

	// Maak hele scherm wit
    volatile alt_u16 *pixelbuffer = 0x08000000;
    for(i=0;i<320*480;i++){
    	pixelbuffer[i] = 0xFFFF;
    }

	// zet kleuren van de kolommen
    colors[0] = 0b1111100000000000;
    colors[1] = 0b1111101110000000;
    colors[2] = 0b1111111111100000;
    colors[3] = 0b0001111111100000;
    colors[4] = 0b0000001111100000;
    colors[5] = 0b0000111000011111;
    colors[6] = 0b0000001111111111;
    colors[7] = 0b0000000001111111;
    colors[8] = 0b0111100000011111;
    colors[9] = 0b1111100000001111;
}

// voor het aanpassen van de kolommen
void DisplayValues(int* values)
{
	int i;
	for(i = 0; i < 10; i++){
		// indien de waarde buiten het scherm zou komen zet op maximale waarde
		if(values[i]>7){
			values[i]=7;
		}
		
		// alleen een kolom aanpassen als deze anders is met wat er daarvoor stond.
		int y;
		if(prevValues[i] > values[i]){ // als de nieuwe waarde kleiner is dan de vorige
			for(y = values[i]; y <= prevValues[i]; y++){
				DisplayBlock(i*32 + 2, (7-y)*32 - 18, 28, 28, 0xFFFF); // maak bovenste blokken wit
			}
		} else if(prevValues[i] < values[i]){ // als de nieuwe waarde groter is dan de vorige
			for(y = prevValues[i]; y < values[i]; y++){
				DisplayBlock(i*32 + 2, (7-y)*32 - 18, 28, 28, colors[i]); // maak de nieuwe blokken de juiste kleur
			}
		}
		prevValues[i] = values[i]; // nieuwe oude waardes zetten
	}
}

// voor het tekenen van 1 blok.
void DisplayBlock(int x, int y, int width, int height, alt_u16 color){
	volatile alt_u16 *pixelbuffer = 0x08000000;
	int x1;
	int y1;
	
	// elke rij op het scherm heeft 512 plekken in het geheugen, maar alleen de eerste 320 worden gebruikt voor het scherm.
	height = height * 512;
	y = y * 512;
	for(x1=x;x1<(width + x); x1++){
		for(y1 = y; y1 < (height + y); y1 += 512){
			pixelbuffer[x1 + y1] = color;
		}
	}
}
