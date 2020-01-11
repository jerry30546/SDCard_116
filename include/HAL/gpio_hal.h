#ifndef HAL_GPIO_HAL_H_
#define HAL_GPIO_HAL_H_

#include <stdint.h>

#define GPIO_INPUT			0
#define GPIO_OUTPUT 		1
#define GPIO_UNSPECIFIED	2

#define GPIO_LEVEL_LOW		0
#define GPIO_LEVEL_HIGH		1

void gpio_drv_init(void);
void gpio_drv_exit(void);
void gpio_set_direction(uint32_t pin, uint8_t direction);
void gpio_set_level(uint32_t pin, uint8_t level);
void gpio_set_toggle(uint32_t index);


#endif /* HAL_GPIO_HAL_H_ */
