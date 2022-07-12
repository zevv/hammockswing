#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/sleep.h>
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

int main2(void)
{
	uart_init(UART_BAUD(19200), 1);
	uart_enable();

	uart_tx('\n');

	motor_init();
	motor_set(18);
	sei();

	for(;;) {
		_delay_ms(10);
		motor_tick_100hz();
		uart_tx('a');
	}



}


int main(void)
{

	uart_init(UART_BAUD(19200), 1);
	timer_init();
	motor_init();
	encoder_init();

	sei();

	printf("---\n");
	uint8_t m = MCUSR;
	if(m & (1<<WDRF))   printf("WDRF\n");
	if(m & (1<<BORF))   printf("BORF\n");
	if(m & (1<<EXTRF))  printf("EXTR\n");
	if(m & (1<<PORF))   printf("PORF\n");
	MCUSR = 0;

	int state = 0;
	int n = 0;
	int t = 0;

	motor_set(35);
	state = 1;

	wdt_enable(WDTO_120MS);

	for(;;) {
		wdt_reset();

		event_t ev;
		event_wait(&ev);

		if(ev.type == EV_UART) {
			//cmd_handle_char(ev.uart.c);
		}
		
		if(ev.type == EV_TICK_1HZ) {
		}

		if(ev.type == EV_TICK_10HZ) {
		}

		if(ev.type == EV_TICK_100HZ) {
		}

		if(ev.type == EV_ENCODER) {

			t += 1;
			n += abs(ev.encoder.speed);

			printf("%d n=%d t=%d s=%d m=%d\n", state, n, t, (int)ev.encoder.speed, OCR1A);

			if(state == 0) {

				if(t > 2 && n > 10 && ev.encoder.speed >= 0) {
					motor_set(64);
					n = 0;
					t = 0;
					state = 1;
				}
			}

			else if(state == 1) {

				if(n > 10 && ev.encoder.speed <= 0) {
					motor_set(0);
					n = 0;
					t = 0;
					state = 0;
				}

			}

			if(t > 16) {
				motor_set(0);
			}
				
			if(abs(n) > 150) {
				motor_set(0);
			}

		}
	}

}

void motor_pid(uint8_t argc, char **argv)
{
}

