#include "HAL/uart_hal.h"

#include <string.h>

#include "lpuart_driver.h"
#include "UART0.h"

void uart_mapping_handler(uint8_t instance);

lpuart_state_t * uart_state;
const lpuart_user_config_t * uart_config;
static uint32_t bRemaining = 0;

void uart_rx_callback_0(void *driverState, uart_event_t event, void *userData) {
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Coding for your application */
    if (event == UART_EVENT_RX_FULL) {

    }
}

void uart_rx_callback_2(void *driverState, uart_event_t event, void *userData) {
    /* Unused parameters */
    (void)driverState;
    (void)userData;

    /* Coding for your application */
    if (event == UART_EVENT_RX_FULL) {

    }
}

void uart_drv_init(uint8_t instance) {
	uart_mapping_handler(instance);
	LPUART_DRV_Init(instance, uart_state, uart_config);
}

void uart_drv_exit(uint8_t instance) {
	LPUART_DRV_Deinit(instance);
}

void uart_set_baudrate(uint8_t instance, uint32_t baudrate) {
	LPUART_DRV_SetBaudRate(instance, baudrate);
}

void uart_send(uint8_t instance, uint8_t *txBuf) {
	while(LPUART_DRV_GetTransmitStatus(instance, &bRemaining) == STATUS_BUSY);
	LPUART_DRV_SendData(instance, txBuf, strlen((char*)txBuf));
}

/**************************************************************************
 * Function Name:		uart_recv
 * Description:			UART non-blocking reception function
 * 						Before using function, please make sure indicate
 * 						receive callback function first.
 * @ param1 instance:	UART port or instance
 * @ param2 rxBuf:		receive buffer
 * @ param3 rxSize:		receive size
 **************************************************************************/
void uart_recv(uint8_t instance, uint8_t *rxBuf, uint32_t rxSize) {
	LPUART_DRV_ReceiveData(instance, rxBuf, rxSize);
}

/**************************************************************************
 * Function Name:		uart_recv
 * Description:			UART blocking reception function
 * @ param1 instance:	UART port or instance
 * @ param2 rxBuf:		receive buffer
 * @ param3 rxSize:		receive size
 **************************************************************************/
void uart_recv_blocking(uint8_t instance, uint8_t *rxBuf, uint32_t rxSize) {
	LPUART_DRV_ReceiveDataBlocking(instance, rxBuf, rxSize, 200UL);
}

void uart_mapping_handler(uint8_t instance) {
	switch(instance) {
	case 0:
		uart_state = &UART0_State;
		uart_config = &UART0_InitConfig0;
		break;
	default:
		break;
	}
}
