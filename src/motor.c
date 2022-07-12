
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "motor.h"


void motor_init(void)
{
	/* Timer 1: ICR1 is top, OCR1[AB] are PWM values */

	TCCR1A = (1<<WGM11);
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	ICR1 = 64;

	/* Set PWM pin OC1A as output */

	DDRB |= (1<<PB1) | (1<<PB2);
	TCCR1A |= (1<<COM1A1);
}


int speed_req = 0;
int speed = 0;


void motor_set(int s)
{
	if(0 && s == 0) {
		speed_req = 0;
		speed = 0;
		OCR1A = 0;
	} else {
		speed_req = s;
	}
}



void motor_tick_100hz(void)
{
	int d = speed_req - speed;
	int s = 3;
	if(d < -s) d = -s;
	if(d >  s) d =  s;
	speed += d;

	OCR1A = speed;

	if(speed > 0) {
		TCCR1A |= (1<<COM1A1);
	} else {
		TCCR1A &= ~(1<<COM1A1);
	}
}


