
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


void motor_set(int speed)
{
//	speed_req = speed;
	OCR1A = speed;
}


void motor_tick_100hz(void)
{
	if(speed_req > speed) {
		speed ++;
		OCR1A = speed;
	}

	if(speed_req < speed) {
		speed --;
		OCR1A = speed;
	}
}


void motor_cmd(uint8_t argc, char **argv)
{
	if(argc > 0) {
		int v = atoi(argv[0]);
		motor_set(v);
	}
}



