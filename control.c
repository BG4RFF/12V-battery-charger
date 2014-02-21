/*
 * control.c
 *
 *  Created on: Apr 14, 2013
 *      Author: dirk-jan
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "control.h"

char err=0;

static char ChargeState=0, new_data = 0;		// charge state
unsigned char pwm_min=0;
static unsigned int resultV=0, resultA=0;

static void Controller(unsigned int ref, unsigned int measured);
static int ADCGetResult(void);
static void ControlSetPWM(unsigned char inputValue);
static void MeasureControl();
static void StateControl();

void ControlInit(){
	//ADC Init.
	ADCSRA = (1<<ADPS0)|(1<<ADPS1)|(1<<ADEN);	//Turn on ADC, with prescaler 8 = 125 KHz @ clk=1MHz
//	ADCSRB = (1<<ADTS2);   						//do a measurement at a Timer/Counter Overflow
	DIDR0 = (1<<ADC2D)|(1<<ADC3D);   			//Disable Digital Input
	ADCGetResult(); 							//eerste meeting doen en data niet gebruiken

	PLLCSR=(1<<PLLE)|(1<<PCKE);				// init PLL, select PCK as clock source
	TCCR1=(1<<CS10)|(1<<PWM1A)|(1<<COM1A1);	// PCK/1 = 64MHz
	OCR1C=255;								// max res
}

unsigned char ControlTask(unsigned char t){// elke ms uitvoeren
	if (t>1) err=2;		// er is meer dan 1 ms voorbij gegaan
	MeasureControl();
	StateControl();
	return ChargeState;
}

char geter(){
	return err;
}

static void Controller(unsigned int ref, unsigned int measured){
	if ((ref>measured)&&(PWM_OUT<255))
		ControlSetPWM(PWM_OUT+1);
	else if  ((ref<measured)&&(PWM_OUT>0))
		ControlSetPWM(PWM_OUT-1);
}

static void MeasureControl(){// elke ms uitvoeren
	static unsigned char state=0;

	switch(state){
		case(0):
			state++;	//Wait a moment for the output to become stable
			break;
		case(1):
			ADMUX=M_V;
			ADCSRA |= (1 << ADSC); 		//start adc conversion
			state++;
			break;
		case(2):
			if (NEWADCDATA)	// als er data klaar staat
				resultV=ADCGetResult();
			else
				err=1;
			state++;
			break;
		case(3):
			ADMUX=M_A;
			ADCSRA |= (1 << ADSC); 		//start adc conversion
			state++;
			break;
		case(4):
			if (NEWADCDATA){	// als er data klaar staat
				resultA=ADCGetResult();
				new_data=1;					// de stroom en spanning zijn nu beide gemeten en kunnen als input voor de controller dienen
			}else
				err=1;
			state=0;
			break;
	}
 }


static void StateControl(){
	static int counter = 0, NoBattCounter = 0, BattCounter = 0;

	if ((resultA>(REF_A*1.2)) || (resultV>(REF_V*1.2)))		// stroom en spanning beveiliging
		ControlSetPWM(0);

	if ((ChargeState>0) && ((resultV<ACCU_V_MIN_CALC)||(resultA<2)))
		NoBattCounter++;

	if (NoBattCounter>50){
		ChargeState=4;
		NoBattCounter=0;
		return;
	}

	if (!new_data) return;
	new_data=0;							// ongeveer elke 5 ms is er nieuwe data

	if (counter>5){
		ChargeState++;
		counter=0;
	}

	switch(ChargeState){
		case(0):		// nothing connected
			if (resultV>=ACCU_V_MIN_CALC) 	counter++; else counter=0;
			ControlSetPWM(0);
			break;
		case(1):		// current charge
			if (resultV>=REF_V) 			counter++; else counter=0;	// als de spanning groter wordt dan 14.7v dan spanningsladen
			Controller(REF_A, resultA);
			break;
		case(2):		// voltage charge
			//laden met 14,7V
			if (resultA<=ACCU_A_MIN_CALC) 	counter++; else counter=0;	//als de stroom lager dan 140 ma wordt dan druppelladen
			Controller(REF_V, resultV);
			break;
		case(3):
			//laden met 13,65V, druppelladen
			Controller(REF_FLOAT, resultV);
			break;
		case(4):
			//wachten tot de condensator ontladen is
			ControlSetPWM(0);
			BattCounter++;
			NoBattCounter=0;
			if (BattCounter>250){
				ChargeState=0;
				BattCounter=0;
			}
			break;
	}
}

static int ADCGetResult(void){
	int result;
	ADCSRA|=(1<<ADIF);	// clear flag
	result=ADC;

	return result;
}

static void ControlSetPWM(unsigned char inputValue){
    if (inputValue<MAX_PWM){
/*    	if (inputValue>(PWM_OUT+MAX_PWM_STEP_UP))
    		PWM_OUT+=MAX_PWM_STEP_UP;
    	else if (inputValue<pwm_min)
    		PWM_OUT=pwm_min;
    	else
    		PWM_OUT=inputValue;*/
    	PWM_OUT=inputValue;
    }else
    	PWM_OUT=MAX_PWM;
}


