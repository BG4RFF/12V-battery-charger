/*
 * main.c
 *
 *  Created on: 14 apr 2013
 *      Author: dirkjan
 *	attiny13 @ 9,6 MHz internal oscillator
 *
 *
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/delay.h>
#include "control.h"
#include "main.h"

static volatile unsigned char timertick=0;
static char ledState=LEDOFF;

ISR(TIM0_COMPA_vect){
	timertick++;
}

void init(){
	redOff();
	DDRB=red_pin|PWM_pin;

	TCCR0A = (1<<WGM01);		// reset timer @ OCR0A
	TCCR0B = (1<<CS01);			// clk/8
								// timer 0 used for timing
	TIMSK = (1<<OCIE0A);		// interrupt enable
	OCR0A = 100;				// @0.1MHz timer clock => 1 ms

	ControlInit();
	sei();
}

void ledTask(unsigned char t){
	static unsigned int ledcounter=0;

	ledcounter+=t;

	switch (ledState){
		case LEDOFF:
			redOff();
			break;
		case LEDON:
			redOn();
			break;
		case LEDBLINK:
			if (ledcounter>=500){
				redToggle();
				ledcounter=0;
			}
			break;
		case LEDERROR:
			if (ledcounter==1500){
				redOn();
			} else if (ledcounter>=1600){
				redOff();
				ledcounter=0;
			}
			break;
		case LEDNOBATT:
			if (ledcounter>=100){
				redToggle();
				ledcounter=0;
			}
			break;
		default:
			ledState=LEDOFF;
			break;
	}
}

int main(void) {
	init();
	unsigned char tmpTick=0, charge_state=0;


	while(1){
		// always execute //

		/*if (geter()==2)
			ledState=LEDERROR;
		else if....*/

		if ((charge_state==0)||(charge_state==4))
			ledState=LEDNOBATT;
		else if (charge_state==1)
			ledState=LEDON;		// stroomladen
		else if (charge_state==2)
			ledState=LEDBLINK;	// spanningladen
		else
			ledState=LEDOFF;



		// end always execute //

		if (timertick){
			tmpTick=timertick;
			timertick-=tmpTick;
			charge_state=ControlTask(tmpTick);
			ledTask(tmpTick);
		}
	}

	return 0;
}
