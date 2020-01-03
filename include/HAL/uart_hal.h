#ifndef HAL_UART_HAL_H_
#define HAL_UART_HAL_H_

#include <stdint.h>

void uart_drv_init(uint8_t instance);
void uart_drv_exit(uint8_t instance);
void uart_set_baudrate(uint8_t instance, uint32_t baudrate);
void uart_send(uint8_t instance, uint8_t *txBuf);
void uart_recv(uint8_t instance, uint8_t *rxBuf, uint32_t rxSize);

#endif /* HAL_UART_HAL_H_ */
