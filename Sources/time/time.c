#include "modules/time/time.h"

#define USE_HARDWARE_TIMER

#ifdef USE_HARDWARE_TIMER
	#include <stddef.h>
	#include "HAL/timer_hal.h"
	const uint8_t timerInstance = 0;
#endif // USE_HARDWARE_TIMER

static volatile uint32_t tickCount = 0;
static uint8_t delayIsEnable = 0;

/**************************************************************************
 * Function Name:		tick_callback
 * Description:			timer callback(interrupt) function
 * @ param1 userData:	callback parameter
 **************************************************************************/
void tick_callback(void *userData) {
	tickCount++;
	(void)userData;
}

/**************************************************************************
 * Function Name:	millis
 * Description:		Get current timing of millisecond
 * Returns: 		millisecond
 **************************************************************************/
uint32_t millis(void) {
	return tickCount;
}

/**************************************************************************
 * Function Name:	time_init
 * Description:		time utilities initial function, initial hardware timer
 * 					or software timer settings
 **************************************************************************/
void time_init(void) {
#ifdef USE_HARDWARE_TIMER
	timer_drv_init(timerInstance);
	timer_install_callback(timerInstance, tick_callback, NULL);
	timer_start(timerInstance, 1000000);
	delayIsEnable = 1;
#endif
}

/**************************************************************************
 * Function Name:	time_exit
 * Description:		Disable time utilities, including disable hardware
 *                  timer
 **************************************************************************/
void time_exit(void) {
	delayIsEnable = 0;
#ifdef USE_HARDWARE_TIMER
	timer_drv_exit(timerInstance);
#endif
}

/**************************************************************************
 * Function Name:	delay_ms
 * Description:		delay millisecond level in blocking
 * @ param1 ms:		delay time
 **************************************************************************/
void delay_ms(uint32_t ms) {
	if(delayIsEnable) {
		uint32_t start = millis();
		while((millis()-start) < ms);
	}
}

/**************************************************************************
 * Function Name:	delay_s
 * Description:		delay second level in blocking
 * @ param1 ms:		delay time
 **************************************************************************/
void delay_s(uint32_t s) {
	if(delayIsEnable) {
		uint32_t start = millis();
		s *= 1000;
		while((millis()-start) < s);
	}
}
