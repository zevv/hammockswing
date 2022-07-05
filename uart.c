
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "event.h"

#define RB_SIZE 160

struct ringbuffer_t {
	uint8_t head;
	uint8_t tail;
	uint8_t buf[RB_SIZE];
};

static volatile struct ringbuffer_t rb = { 0, 0 };

/*
 * Initialize the UART. 
 */

void uart_init(uint16_t baudrate_divider, uint8_t stop_bits)
{
	rb.head = rb.tail = 0;

	/* Set baudrate, no parity, 8 databits 1 stopbit */

	UBRR0H = (baudrate_divider >> 8);
	UBRR0L = (baudrate_divider & 0xff);			
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (stop_bits == 2 ? (1<<USBS0) : 0);

	/* Enable receiver and transmitter and rx interrupts */

	UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);	
}


void uart_enable(void)
{
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);	
}


void uart_disable(void)
{
	UCSR0B = 0;
}


/*
 * Write one character to the transmit ringbuffer and enable TX-reg-empty interrupt
 */ 
 
void uart_tx(uint8_t c)
{
	uint8_t head;
	head = (rb.head + 1) % RB_SIZE;

	if(head == rb.tail) rb.tail = (rb.tail + 1) % RB_SIZE;
	// while(rb.tail == head);

	rb.buf[rb.head] = c;
	rb.head = head;

	/* Enable TX-reg-empty interrupt */

	UCSR0B |= (1<<UDRIE0);
}


ISR(USART_RX_vect)
{
	event_t ev;
	ev.type = EV_UART;
	ev.uart.c = UDR0;
	event_push(&ev);
}


ISR(USART_UDRE_vect)
{
	UDR0 = rb.buf[rb.tail];
	rb.tail = (rb.tail + 1) % RB_SIZE;
	
	if(rb.tail == rb.head) {
		UCSR0B &= ~(1<<UDRIE0);
	}
}


/*
 * End
 */

