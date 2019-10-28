/*****************************************************************************
//  File Name    : main.c
//  Version      : 1.0
//
*****************************************************************************/
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <avr/iom640.h>

#include "uart_printf.h"
#include "timer.h"


#define IR_CAPTURE_IN (1 << PINB5)

#define MAX_SAMPLES_NB 100
#define ESP32_NATIVE 1


uint16_t Transitions[MAX_SAMPLES_NB];
volatile uint16_t transitionIndex;
volatile uint8_t done;

void enableIrInterrupt(void){
	PCMSK0 |= (1<<PCINT5);
	PCICR |= (1<<PCIE0);
}



ISR(PCINT0_vect)
{
	unsigned short tCnt;	
	if ( done == 1 ){
		TCNT3 = 0;
		transitionIndex = 0;
		done = 0;
	}
	tCnt = TCNT3;
	
	if ( (done ==0) && (transitionIndex < MAX_SAMPLES_NB) )
	{
		Transitions[transitionIndex]=tCnt;
		//if ( PINB & (1 << PINB5) )
		//	irPinHigh++;
		transitionIndex++;
	}
}


void timer0Setup(void){
	// IT Timer0 Over Flow Active 
	TIMSK0=(1<<TOIE0);
	// Prescaler 1024 (Clock/1024) 
	TCCR0B = ((1 << CS02) | (0 << CS01) | (1 << CS00)); // Start timer at Fcpu/1024
}


void timer3Setup(void){
	// IT Timer1 Over Flow Active 
	TIMSK3=(1<<TOIE3);
	// Prescaler 64 (Clock/64) 
	TCCR3B = ((0 << CS32) | (1 << CS31) | (1 << CS30)); // Start timer at Fcpu/64 (250KHz)
	//TCCR3B = ((1 << CS32) | (0 << CS31) | (0 << CS30)); // Start timer at Fcpu/256
	//TCCR3B = ((1 << CS32) | (0 << CS31) | (1 << CS30)); // Start timer at Fcpu/1024
}



ISR(TIMER3_OVF_vect){ 
	// Time out
	done = 1;
}

uint32_t fixTimeScale( uint32_t t ){	
#ifdef ESP32_NATIVE
	uint32_t tt;
	tt = (t * 16)/5;
	//if ( tt > 32767 )
	//	tt = 32767;
	return tt;
#else	
	return (t+2)/5;  // Target is 50Kz (20us)
#endif	
}

/* expected output format:
	{
		.length = 10,
		.rawTxItems = {
			{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
		},
	},
	***********************************************/
#if ESP32_NATIVE	
// Native ESP32 format
//    {{{ 32767, 1, 32767, 0 }}}, // dot

int dumpTransitionsToC( int index )
{
	int i;
	uint32_t h, l, delay;
	
	if  ( index < 2 ){
		return 0;
	}
	
	printf( "    {\n");	
	for ( i=0 ; i< index ; i+=2 ){
		h = fixTimeScale(Transitions[i+1] - Transitions[i]);
		l = fixTimeScale(Transitions[i+2] - Transitions[i+1]);
		
		if ( l > 32767 )
		{
			delay = l - 32767;
			l = 32767;
		}
		else {
			delay = 0;
		}		
		printf( "        {{{ %5lu, 1, %5lu, 0 }}}, // %u\n", h, l, i );
		if ( delay > 0 ){  // TODO loop
			printf( "        {{{ %5lu, 0, %5lu, 0 }}},\n", delay/4, delay/4 );
			printf( "        {{{ %5lu, 0, %5lu, 0 }}},\n", delay/4, delay/4 );
		}
	}
	printf( "    {\n");
	return 0;
}

#else
int dumpTransitionsToC( int index )
{
	int i;
	uint16_t h, l;
	
	if  ( index <= 0 ){
		return 0;
	}
	
	printf( "    {\n");
	printf( "        .length = %d,\n", index/2);
	printf( "        .rawTxItems = {");
	
	
	for ( i=0 ; i< index ; i+=2 ){
		if ( i%16 == 0 )
			printf( "\n            ");
		h = fixTimeScale(Transitions[i+1] - Transitions[i]);
		l = fixTimeScale(Transitions[i+2] - Transitions[i+1]);
		printf( "{%3u,%3u}, ", h, l );		
	}
	printf( "\n        },\n");
	printf( "    },\n");
	return 0;
}
#endif

int dumpTransitions( int index )
{
	int i;
	for ( i=0 ; i< index ; i++ ){
		printf( "Transitions %d = %5u\n", i,  Transitions[i] );
	}
	return 0;
}

int main(void){ 
	timer1Setup();
	timer3Setup();
  
	enableIrInterrupt();
	PORTB |= IR_CAPTURE_IN;  // PULL-up resitor
  
//Configuration PORTB.7 en sortie 
	DDRB |= (1<<DDB7);   
	PORTB &= ~(1<<PB7); // PORTB.4 <-0 
//activation des IT (SREG.7=1) 
	
	sei(); 
// SREG |= 0x80; // équivalent à sei()

    // Initialize UART print
    uart_printf_init();
    
    printf("HelloAVR\n\n");

    while ( 1 )
    {
		MsSleep(200);
		if ( done && transitionIndex>2 ){
			dumpTransitionsToC(transitionIndex);
			printf("Nb pulse = %u\n", transitionIndex/2 );			
			printf("Frame length = %lu\n", fixTimeScale(Transitions[transitionIndex-2]) );
			//dumpTransitions(transitionIndex);
			transitionIndex = 0;
		}
		//printf("irpin=%u: %u/%u\n", instantTime, irPinHigh, irPinLow );
		//printf("irpin=%u: %u/%u\n", instantTime, irPinHigh, irPinLow );
		//printf("--\n" );		
		//printf("done = %d\n", (int)done);
		if ( ! done )
			PORTB |=  (1<<PB7);
		else
			PORTB &= ~(1<<PB7);
    }

}



/* EOF  */



