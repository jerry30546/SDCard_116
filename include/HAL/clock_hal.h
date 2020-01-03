#ifndef HAL_CLOCK_HAL_H_
#define HAL_CLOCK_HAL_H_

#include <stdint.h>

void clock_drv_init(void);
void clock_drv_exit(void);
void clock_set_system(uint32_t freq);
uint32_t clock_get_system(void);

#endif /* HAL_CLOCK_HAL_H_ */
