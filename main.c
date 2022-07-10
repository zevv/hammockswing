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

static uint8_t go_to_bootloader __attribute__ ((section (".noinit")));

void cmd_handle_char(uint8_t c);

static void init_bootloader(void)
		 __attribute__ ((naked))
		 __attribute__ ((used))
		 __attribute__ ((section (".init3")));

static void init_bootloader(void)
{
	if(go_to_bootloader && (MCUSR & (1 << WDRF))) {
		go_to_bootloader = 0;
		MCUSR = (1<<EXTRF);
		((void (*)(void))0x7f00)();
		for(;;);
	}
}


int main(void)
{

	wdt_disable();

	uart_init(UART_BAUD(38400), 1);
	uart_enable();
	timer_init();
	motor_init();
	encoder_init();

	wdt_disable();

	sei();

	printf("---\n");

	uint8_t m = MCUSR;
	if(m & (1<<WDRF))   printf("WDRF\n");
	if(m & (1<<WDRF))   printf("WDRF\n");
	if(m & (1<<BORF))   printf("BORF\n");
	if(m & (1<<EXTRF))  printf("EXTR\n");
	if(m & (1<<PORF))   printf("PORF\n");
	MCUSR = 0;


	int state = 0;
	int n = 0;
	int t = 0;

	//int high = 64;
	//int low = 10;
	//
	
	motor_set(64);
	state = 1;

	for(;;) {
		wdt_reset();

		event_t ev;
		event_wait(&ev);

		if(ev.type == EV_UART) {
			//cmd_handle_char(ev.uart.c);
			go_to_bootloader = 1;
			wdt_enable(WDTO_250MS);
			for (;;);
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
			printf("%d n=%d t=%d s=%d\n", state, n, t, (int)ev.encoder.speed);


			if(state == 0) {

				if(t > 2 && n > 10 && ev.encoder.speed == 0) {
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

