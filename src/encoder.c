
#include <avr/interrupt.h>
#include <stdio.h>

#include "encoder.h"
#include "event.h"


static volatile int32_t count = 0;


void encoder_init(void)
{
	EICRA |= (1<<ISC01);
	EIMSK |= (1<<INT0);
}



void encoder_tick_10hz(void)
{
	event_t ev;
	ev.type = EV_ENCODER;
	ev.encoder.speed = count;
	event_push(&ev);

	count = 0;
}



ISR(INT0_vect)
{
	if(PIND & (1<<PD3)) {
		count ++;
	} else {
		count --;
	}
}




