/*
 * control.h
 *
 *  Created on: Apr 14, 2013
 *      Author: dirk-jan
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#define PWM_OUT		OCR1A				//output of control system
#define M_V			(1<<MUX0)|(1<<MUX1)|(1<<REFS1)|(1<<REFS2) 	//measure voltage  	(1<<ADC3) // voor extern Vref add |(1<<REFS0)
#define M_A			(1<<MUX1)|(1<<REFS1)|(1<<REFS2) 			//measure ampere

#define NEWADCDATA	(ADCSRA&(1<<ADIF))


#define MAX_PWM         	254		// max pwm value
//#define MAX_PWM_STEP_UP		3
#define V_REF				2.56		// voedingsspanning of spanning op pin5 (PB0) [V]

//todo waardes aanpassen
#define CHARGE_CURRENT 		0.600	// [A]	600 mA
#define CHARGE_CURRENT_MIN	0.33	// [A]	140 mA
#define CHARGE_VOLTAGE 		14.5	// [V]
#define CHARGE_FLOAT 		13.65	// [V]
#define ACCU_V_MIN			10		// accu detecteren bij deze spanning

#define ACCU_V_MIN_CALC		(const int)((ACCU_V_MIN/6.6*1024)/V_REF)				// accu detecteren bij deze spanning
#define REF_A          		(const int)(((CHARGE_CURRENT*0.22*30-2.043)*1024)/V_REF)		// laadstroom
#define ACCU_A_MIN_CALC		(const int)(((CHARGE_CURRENT_MIN*0.22*30-2.043)*1024)/V_REF)	// als de stroom lager dan 140 ma wordt dan druppelladen
#define REF_V          		(const int)((CHARGE_VOLTAGE/6.6*1024)/V_REF)			// 26 druppellaadspanning
#define REF_FLOAT      		(const int)((CHARGE_FLOAT/6.6*1024)/V_REF)				// float spanning


void ControlInit();
unsigned char ControlTask(unsigned char t);	// geeft chargestate terug
char geter();


#endif /* CONTROL_ */
