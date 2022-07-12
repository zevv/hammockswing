#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cmd.h"
#include "uart.h"
#include "motor.h"

struct cmd_handler {
	char cmd;
	void (*fn)(uint8_t, char **);
};

void motor_pid(uint8_t argc, char **argv);


struct cmd_handler cmd_handler_list[] = {
	{ 'm', motor_cmd },
	{ 'p', motor_pid },
};

#define CMD_COUNT (sizeof(cmd_handler_list) / sizeof(cmd_handler_list[0]))


/*
 * Main serial command loop handler
 */

void cmd_handle_char(uint8_t c)
{
	static char buf[32] = "";
	static uint8_t l = 0;
	char *part[8];
	uint8_t parts=0;
	char *p1, *p2;
	uint8_t i;

	if(c == '\b' || c == 127) {
		if(l>0) {
			buf[--l] = 0;
			//printf("\b \b");
		}
	}

	if(isprint(c)) {
		buf[l] = c;
		if(l < sizeof buf - 1) {
			l++;
			buf[l] = 0;
			uart_tx(c);
		}
	}

	if((c == '\n') || (c == '\r')) {
		uart_tx('\n');
		p1 = buf;
		part[parts++] = p1;

		while((p2 = strchr(p1, ' ')) &&  (parts<8)) {
			*p2=0;
			p1 = p2+1;
			part[parts++]=p1;
		}

		for(i=0; i<CMD_COUNT; i++) {
			if(part[0][0] == cmd_handler_list[i].cmd) {
				cmd_handler_list[i].fn(parts-1, part+1);
			}
		}

		l = 0;
	}
}



