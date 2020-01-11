#ifndef MODULES_TIME_TIME_H_
#define MODULES_TIME_TIME_H_

#include <stdint.h>

void time_init(void);
void time_exit(void);

uint32_t millis();

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);

#endif /* MODULES_TIME_TIME_H_ */
