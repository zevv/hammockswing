
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


float power_cur = 0;
float power_req = 0;
float power_delta = 0;


void motor_set(int req)
{
	power_cur = req;
	power_req = req;
	power_delta = 0;
}

void motor_goto(int req, float dt)
{
	power_req = req;
	power_delta = (power_req - power_cur) / dt * 0.01;
}



void motor_tick_100hz(void)
{
	if(power_delta > 0 && power_cur < power_req) {
		power_cur += power_delta;
	}

	if(power_delta < 0 && power_cur > power_req) {
		power_cur += power_delta;
	}

	if(power_cur > 0) {
		OCR1A = power_cur;
		TCCR1A |= (1<<COM1A1);
	} else {
		TCCR1A &= ~(1<<COM1A1);
	}
}


