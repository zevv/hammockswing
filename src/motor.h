#ifndef motor_h
#define motor_h


void motor_init(void);
void motor_set(int speed);
void motor_tick_100hz(void);
void motor_tick_1000hz(void);
void motor_cmd(uint8_t argc, char **argv);

#endif
