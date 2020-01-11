#include "HAL/timer_hal.h"

#include "timing_pal.h"
#include "TIMER0.h"
#include "TIMER1.h"

const timing_instance_t* timer_instance;
timer_config_t* timer_config;

void timer_mapping_handler(uint8_t instance);

void timer_drv_init(uint8_t instance) {
	timer_mapping_handler(instance);
	TIMING_Init(timer_instance, timer_config);
}

void timer_drv_exit(uint8_t instance) {
	timer_stop(instance);
	TIMING_Deinit(timer_instance);
}

void timer_install_callback(uint8_t instance, timer_callback callback,
		void *userData) {
	timer_mapping_handler(instance);
	TIMING_InstallCallback(timer_instance, 0, (timer_callback_t)callback,
			userData);
}

uint8_t timer_start(uint8_t instance, uint64_t trigTime) {
	uint64_t timerResolution;
	uint32_t periodTicks;

	timer_mapping_handler(instance);
	TIMING_GetResolution(timer_instance,
			TIMER_RESOLUTION_TYPE_NANOSECOND, &timerResolution);
	periodTicks = (uint32_t)(trigTime/timerResolution);
	if(trigTime < timerResolution ||
			((periodTicks > 65535) && (timer_instance->instType != TIMING_INST_TYPE_LPIT))) {
		return TIMER_STATUS_ERROR;
	}
	TIMING_StartChannel(timer_instance, 0,
			(uint32_t)(trigTime/timerResolution));
	return TIMER_STATUS_OK;
}

void timer_stop(uint8_t instance) {
	timer_mapping_handler(instance);
	TIMING_StopChannel(timer_instance, 0);
}

void timer_mapping_handler(uint8_t instance) {
	switch(instance) {
	case 0:
		timer_instance = &TIMER0_instance;
		timer_config = &TIMER0_InitConfig;
		break;
	case 1:
		timer_instance = &TIMER1_instance;
		timer_config = &TIMER1_InitConfig;
		break;
	}
}

uint32_t timer_get_ticks(uint8_t instance) {
	timer_mapping_handler(instance);
	return TIMING_GetRemaining(timer_instance, 0);
}
