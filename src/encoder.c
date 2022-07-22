
#include <avr/interrupt.h>
#include <stdio.h>

#include "encoder.h"
#include "event.h"


static volatile int32_t count = 0;


void encoder_init(void)
{
	/* Configure pin change interrupt */

	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT18);

}



void encoder_tick_10hz(void)
{
	event_t ev;
	ev.type = EV_ENCODER;
	ev.encoder.speed = count;
	event_push(&ev);

	count = 0;
}



ISR(PCINT2_vect)
{
	if(PIND & (1<<PD3)) {
		if(PIND & (1<<PD2)) {
			count ++;
		} else {
			count --;
		}
	}
}




