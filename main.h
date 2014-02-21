/*
 * main.h
 *
 *  Created on: Apr 14, 2013
 *      Author: dirk-jan
 */

#ifndef MAIN_H_
#define MAIN_H_


#define red_pin			(1<<PB2)
#define PWM_pin			(1<<PB1)	// OC1A
#define	VM_pin			(1<<PB3)	// ADC3
#define AM_pin			(1<<PB4)	// ADC2

#define redOn()			PORTB &=~red_pin
#define redOff()		PORTB |= red_pin
#define redToggle()		PORTB ^= red_pin

#define LEDOFF			0
#define LEDON			1
#define LEDBLINK		2
#define LEDERROR		3
#define LEDNOBATT		4



#endif /* MAIN_H_ */
