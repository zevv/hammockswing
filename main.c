#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <inttypes.h>

#include "uart.h"
#include "event.h"
#include "timer.h"
#include "cmd.h"
#include "encoder.h"
#include "motor.h"

void cmd_handle_char(uint8_t c);


int main(void)
{

	uart_init(UART_BAUD(9600), 1);
	uart_enable();
	timer_init();
	motor_init();
	encoder_init();

	sei();

	for(;;) {
		event_t ev;
		event_wait(&ev);

		if(ev.type == EV_UART) {
			cmd_handle_char(ev.uart.c);
		}
		
		if(ev.type == EV_TICK_1HZ) {
		}

		if(ev.type == EV_TICK_10HZ) {
		}

		if(ev.type == EV_TICK_100HZ) {
		}
		
		if(ev.type == EV_ENCODER) {
			printf("%" PRId32 "\n", ev.encoder.speed);
		}
	}

}


