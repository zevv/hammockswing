#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "uart.h"
#include "event.h"
#include "timer.h"
#include "cmd.h"

void motor_init(void);
void motor_set(int speed);
void cmd_handle_char(uint8_t c);

int main(void)
{

	uart_init(UART_BAUD(9600), 1);
	uart_enable();

	timer_init();

	motor_init();

	sei();

	for(;;) {
		event_t ev;
		event_wait(&ev);
		
		if(ev.type == EV_UART) {
			cmd_handle_char(ev.uart.c);
		}

		if(ev.type == EV_TICK_10HZ) {
			printf("t");
		}
	}

}




void motor_init(void)
{

	/* Timer 1: ICR1 is top, OCR1[AB] are PWM values */

	TCCR1A = (1<<WGM11);
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS10);
	ICR1 = 64;

	/* Set PWM pins OC1A/OC1B/OC2B as output */

	DDRB |= (1<<PB1) | (1<<PB2);

	/* Set FAN pulse pins to input, pullup enabled */

	PORTD |= (1<<PD7) | (1<<PD6);

	/* Set PWM */

	TCCR1A |= (1<<COM1A1);
}


void motor_set(int speed)
{
	OCR1A = speed;
}


void cmd_motor(uint8_t argc, char **argv)
{
	if(argc > 0) {
		int v = atoi(argv[0]);
		motor_set(v);
	}
}


