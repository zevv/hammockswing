
#ifndef event_h
#define event_h

#include <stdint.h>

enum evtype {
	EV_TICK_1HZ,
	EV_TICK_10HZ,
	EV_TICK_100HZ,
	EV_ENCODER,
	EV_UART,
};

struct ev_tick_1hz {
	enum evtype type;
};

struct ev_tick_10hz {
	enum evtype type;
};

struct ev_tick_100hz {
	enum evtype type;
};

struct ev_encoder {
	enum evtype type;
	int32_t speed;
};

struct ev_uart {
	enum evtype type;
	uint8_t c;
};

typedef union {
	enum evtype type;
	struct ev_tick_1hz tick_1hz;
	struct ev_tick_10hz tick_10hz;
	struct ev_tick_100hz tick_100hz;
	struct ev_encoder encoder;
	struct ev_uart uart;
} event_t;

void event_push(event_t *ev);
void event_wait(event_t *ev);
uint8_t event_pop(event_t *ev);

#endif
