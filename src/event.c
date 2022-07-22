#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "event.h"

#define EVQ_SIZE 16

struct evq {
	uint8_t head;
	uint8_t tail;
	event_t list[EVQ_SIZE];
};

static volatile struct evq evq;


void event_init(void)
{
}


void event_push(event_t *ev)
{
	uint8_t s = SREG; cli();

	uint8_t newhead;
	evq.list[evq.head] = *ev;
	newhead = (evq.head + 1) % EVQ_SIZE;
	if(newhead != evq.tail) evq.head = newhead;

	SREG = s;
}


uint8_t event_pop(event_t *ev)
{
	uint8_t s = SREG; cli();

	if(evq.head == evq.tail) return 0;
	*ev = evq.list[evq.tail];
	evq.tail = (evq.tail + 1) % EVQ_SIZE;

	SREG = s;

	return 1;
}


void event_wait(event_t *ev)
{
	while(evq.head == evq.tail) {
		set_sleep_mode(SLEEP_MODE_IDLE);
	}
	event_pop(ev);
}

/*
 * End
 */


