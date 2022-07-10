
#include <avr/io.h>
#include <stdlib.h>

#include "motor.h"


void motor_init(void)
{
	/* Timer 1: ICR1 is top, OCR1[AB] are PWM values */

	TCCR1A = (1<<WGM11);
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS10);
	ICR1 = 64;

	/* Set PWM pin OC1A as output */

	DDRB |= (1<<PB1) | (1<<PB2);
	TCCR1A |= (1<<COM1A1);
}


int speed_req = 0;
int speed = 0;


void motor_set(int s)
{
	if(s > 0) {
		speed_req = s;
	} else {
		speed_req = s;
		speed = s;
		OCR1A = s;
	}
}


void motor_tick_100hz(void)
{
	int d = speed_req - speed;
	if(d < -2) d = -2;
	if(d >  2) d =  2;

	speed += d;
	OCR1A = speed;
}


void motor_cmd(uint8_t argc, char **argv)
{
	if(argc > 0) {
		int v = atoi(argv[0]);
		motor_set(v);
	}
}



