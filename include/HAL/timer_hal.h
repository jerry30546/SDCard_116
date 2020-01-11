#ifndef HAL_TIMER_HAL_H_
#define HAL_TIMER_HAL_H_

#include <stdint.h>

#define TIMER_STATUS_OK		0
#define TIMER_STATUS_ERROR	1

typedef void (*timer_callback)(void *userData);

void timer_drv_init(uint8_t instance);
void timer_drv_exit(uint8_t instance);
void timer_install_callback(uint8_t instance, timer_callback callback,
		void *userData);
uint8_t timer_start(uint8_t instance, uint64_t trigTime);
void timer_stop(uint8_t instance);

uint32_t timer_get_ticks(uint8_t instance);

#endif /* HAL_TIMER_HAL_H_ */
