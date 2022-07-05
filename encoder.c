
#include <avr/interrupt.h>

#include "encoder.h"
#include "event.h"


static volatile int32_t count = 0;


void encoder_init(void)
{
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT22);
}



void encoder_tick_10hz(void)
{
	event_t ev;
	ev.type = EV_ENCODER;
	ev.encoder.speed = count * 10;
	event_push(&ev);

	count = 0;
}



ISR(PCINT2_vect)
{
	if(PIND & (1<<PD7)) {
		count ++;
	} else {
		count --;
	}
}




