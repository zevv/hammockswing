
#ifndef event_h
#define event_h

#include <stdint.h>

enum evtype {
	EV_TICK_1HZ,
	EV_TICK_10HZ,
	EV_ERROR,
	EV_UART,
};

struct ev_tick_1hz {
	enum evtype type;
};

struct ev_tick_10hz {
	enum evtype type;
};

enum error_code {
	ERROR_T_MAX         = 0x01,
	ERROR_DT_MIN        = 0x02,
	ERROR_FAN_SPEED     = 0x04,
	ERROR_CASEFAN_SPEED = 0x08,
};

struct ev_error {
	enum evtype type;
	uint8_t error_mask;
};

struct ev_uart {
	enum evtype type;
	uint8_t c;
};

typedef union {
	enum evtype type;
	struct ev_tick_1hz tick_1hz;
	struct ev_tick_10hz tick_10hz;
	struct ev_error error;
	struct ev_uart uart;
} event_t;

void event_push(event_t *ev);
void event_wait(event_t *ev);
uint8_t event_pop(event_t *ev);

#endif
