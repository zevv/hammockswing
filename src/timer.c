#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "event.h"
#include "encoder.h"
#include "motor.h"

/* 
 * Timer tick handlers are defined in main.c 
 * */

extern void timer_isr_100hz(void);
extern void timer_isr_10hz(void);
extern void timer_isr_1hz(void);


/*
 * Setup timer 0 for 1Khz operation and configure OC0B for PWM control of the
 * lighting LED
 */

void timer_init(void)
{
	TCCR0A = (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
	TCCR0B = (1<<CS01) | (1<<CS00);
	TIMSK0 |= (1<<TOIE0);
}


/*
 * Timer0 overflow interrupt @ 500 Hz
 */

ISR(TIMER0_OVF_vect)
{

	static uint8_t t100 = 0;
	static uint8_t t10 = 0;
	static uint8_t t1 = 0;

	if(t100-- == 0) {
		timer_isr_100hz();
		t100 = 4;

		if(t10-- == 0) {
			timer_isr_10hz();
			t10 = 9;

			if(t1-- == 0) {
				timer_isr_1hz();
				t1 = 9;
			}
		}
	}
}




void timer_isr_100hz(void)
{
	motor_tick_100hz();

//	event_t ev;
//	ev.type = EV_TICK_100HZ;
//	event_push(&ev);
}


void timer_isr_10hz(void)
{
	encoder_tick_10hz();

	event_t ev;
	ev.type = EV_TICK_10HZ;
	event_push(&ev);
}


void timer_isr_1hz(void)
{
	event_t ev;
	ev.type = EV_TICK_1HZ;
	event_push(&ev);
}

/*
 * End
 */


