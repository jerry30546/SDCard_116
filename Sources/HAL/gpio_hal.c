#include "HAL/gpio_hal.h"

#include "S32K116.h"
#include "pins_driver.h"
#include "pin_mux.h"

GPIO_Type *port;
uint8_t pin;
void mapping_handler(uint32_t index);

void gpio_drv_init(void) {
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
}

void gpio_set_direction(uint32_t index, uint8_t direction) {
	mapping_handler(index);
	PINS_DRV_SetPinDirection(port, pin, direction);
}

void gpio_set_level(uint32_t index, uint8_t level) {
	mapping_handler(index);

	if(level == GPIO_LEVEL_HIGH) {
		PINS_DRV_SetPins(port, 1 << pin);
	}

	if(level == GPIO_LEVEL_LOW) {
		PINS_DRV_ClearPins(port, 1 << pin);
	}
}

void mapping_handler(uint32_t index) {
	uint8_t port_count = index/32;
	pin = index%32;

	switch(port_count) {
	case 0:
		port = PTA;
		break;
	case 1:
		port = PTB;
		break;
	case 2:
		port = PTC;
		break;
	case 3:
		port = PTD;
		break;
	case 4:
		port = PTE;
		break;
	}
}

/****************************************************************
 *                    Pins Map for NXP S32K1xx
 * --------------------------------------------------------------
 * Index Number:    0-31 => Port A 0-32
 * Index Number:   32-63 => Port B 0-32
 * Index Number:   64-95 => Port C 0-32
 * Index Number:  96-127 => Port D 0-32
 * Index Number: 128-159 => Port E 0-32
 *
 ****************************************************************/
