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


enum state {
	STATE_PULL_START,
	STATE_PULL_END,
	STATE_SWING_START,
	STATE_SWING_END,
	STATE_POWERDOWN,
};


enum state state;
int power_max = 64; // how high?
int power_min = 32; // just enough to keep running
int n = 0;
int t = 0;
int h = 100;



void pull_start_to();
void pull_end_to();
void swing_start_to();
void swing_end_to();
void swing_powerdown_to();

void to(enum state s)
{
	if(state != s) {
		state = s;
		t = 0;

		if(state == STATE_PULL_START) pull_start_to();
		if(state == STATE_PULL_END) pull_end_to();
		if(state == STATE_SWING_START) swing_start_to();
		if(state == STATE_SWING_END) swing_end_to();
		if(state == STATE_POWERDOWN) swing_powerdown_to();
	}
}


void pull_start_to()
{
	n = 0;
}

void pull_start_do(int speed)
{
	// reduce power before reaching stall
	if(n > h) {
		to(STATE_PULL_END);
	}
}

void pull_end_to()
{
	motor_goto(power_min, 0.3);
}

void pull_end_do(int speed)
{
	// detected stall, reverse
	if(n > 10 && speed <= 0) {
		h = n * 0.9;
		to(STATE_SWING_START);
	}
}

void swing_start_to()
{
	n = 0;
	motor_set(0);
}

void swing_start_do(int speed)
{
	// Almost end of swing reached
	if(n > h) {
		to(STATE_SWING_END);
	}
}

void swing_end_to()
{
	// start pulling
	motor_set(power_min);
	motor_goto(power_max, 0.5);
}

void swing_end_do(int speed)
{
	// detected stall, reverse
	if(t > 2 && n > 10 && speed >= 0) {
		h = n * 0.8;
		to(STATE_PULL_START);
	}
}


void swing_powerdown_to()
{
	printf("\npowerdown\n");
	motor_set(0);
	n = 0;
}


void powerdown_do(int speed)
{
	if(speed != 0) {
		to(STATE_SWING_END);
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


	motor_set(40);
	state = STATE_PULL_END;

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

			extern float power_cur;
			extern float power_req;

			printf("%d n=%3d h=%3d t=3%d s=%+3d  cur=%2d req=%2d\n", 
					state, n, h, t, (int)ev.encoder.speed,
					(int)power_cur,
					(int)power_req);

			int speed = ev.encoder.speed;

			if(state == STATE_PULL_START) pull_start_do(speed);
			if(state == STATE_PULL_END) pull_end_do(speed);
			if(state == STATE_SWING_START) swing_start_do(speed);
			if(state == STATE_SWING_END) swing_end_do(speed);
			if(state == STATE_POWERDOWN) powerdown_do(speed);
			
			// abort if running too long
			if(t > 20) {
				to(STATE_POWERDOWN);
			}
			
			// abort if running too far
			if(abs(n) > 350) {
				to(STATE_POWERDOWN);
			}

		}
	}

}

void motor_pid(uint8_t argc, char **argv)
{
}
