/*
 ============================================================================
 Name        : IrCapture.cpp
 Author      : gecuy
 Version     :
 Copyright   : Free sofware
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*   Capture Audio sample @48KHz via arecord:
 *
 *   usage: arecord -v -r 48000 -t raw -f U8 | ./IrCapture
 *   
 * output sample rate 50Khz (See fixTimeScale)
 * to make calculation easier in esp firmware
 ************************************************************/

#define MAX_SAMPLES_NB   250
#define TAIL_SAMPLE_VALUE 100

//#define TIME_OUT_VALUE 2500
#define TIME_OUT_VALUE 255

uint16_t Transitions[MAX_SAMPLES_NB];



int byteToSample( uint8_t byte ){
	int sample;

	sample = (int)byte;
	sample -= 128;

	if ( sample & 0x80 )
		sample |= 0xFFFFFF00;

	return sample;
}

int fixTimeScale( int t ){
	if ( t == TIME_OUT_VALUE )
		return TAIL_SAMPLE_VALUE;
	// Perform floating calculation on host
	double f = (double)t;
	double ratio = (50.E3)/(48.E3);
	
	// Unit is 20 us instead of 20.8333
	
	int fixed = (int)( 0.5 + f*ratio );
	
	return fixed;
}


int dumpTransitions( int index )
{
	for ( int i=0 ; i< index ; i++ ){
		printf( "Transitions %d = %d\n", i&1,  (int)Transitions[i] );
	}
}

/* expected output format:
	{
		.length = 10,
		.rawTxItems = {
			{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
		},
	},
	***********************************************/
int dumpTransitionsToC( int index )
{
	int h, l;
	printf( "    {\n");
	printf( "        .length = %d,\n", index/2);
	printf( "        .rawTxItems = {");

	for ( int i=0 ; i< index ; i+=2 ){
		if ( i%16 == 0 )
			printf( "\n            ");
		h = fixTimeScale( (int)Transitions[i] );
		l = fixTimeScale( (int)Transitions[i+1] );
		printf( "{%3d,%3d}, ", h, l );
	}
	printf( "\n        },\n");
	printf( "    },\n");
}


int main( int argc, char * argv[] )
{
	FILE * fIn;
	char * InputFileName = NULL;

	uint8_t byte;
	int sample;
	int ret;
	int started;
	int transitionIndex=0;
	int levelCounter=0;
	int timeOutCounter=0;
	int sampleCounter=0;
	int prevLevel=0;


	if ( argc > 1 ){
		InputFileName = argv[1];
		fIn = fopen( InputFileName, "r" );
		if ( fIn == NULL ){
			fprintf( stderr, "Cannot find %s\n", InputFileName );
			return -1;
		}
	} else {
		fIn = stdin;
	}


	started = 0;
	while( 1 ){
		ret = fread( &byte, 1, 1, fIn );
		if ( ret <= 0 )
			break;
		sampleCounter++;
		sample = byteToSample(byte);
		int level = (int)(sample>25);
		//if ( level )
		//printf( "byte = %02X -> %+d -> %d\n", (unsigned)byte, sample, level );

		if ( (started==0) && (level > 0) ){
			printf( "Started: %d\n", sampleCounter);
			started = 1;
			transitionIndex = 0;
			levelCounter = 0;
			prevLevel = 1;
		}

		if ( started ){
			if ( timeOutCounter > TIME_OUT_VALUE ){
				printf( "Time out\n");
				Transitions[transitionIndex++] = TIME_OUT_VALUE;
				//dumpTransitions(transitionIndex);
				dumpTransitionsToC(transitionIndex);
				started=0;
				timeOutCounter=0;
				prevLevel = 1;
			}
			if  ( level != prevLevel ) {
				//printf( "Transition [%d]=%d ...\n", transitionIndex, levelCounter);
				Transitions[transitionIndex++] = levelCounter;
				levelCounter = 0;
				timeOutCounter=0;
				prevLevel = level;
			}
			levelCounter++;
			timeOutCounter++;
			if ( transitionIndex >= MAX_SAMPLES_NB )
				break;
		}

	}
	printf( "Total sample counter %d (%f sec)\n", sampleCounter, (double)sampleCounter / 48E3 );

	if ( InputFileName != NULL )
		fclose(fIn);
    return EXIT_SUCCESS;
}
