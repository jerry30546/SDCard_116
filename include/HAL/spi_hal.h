#ifndef HAL_SPI_HAL_H_
#define HAL_SPI_HAL_H_

#include <stdint.h>

#define SPI_MASTER_MODE		0
#define SPI_SLAVE_MODE		1
#undef SPI_HARDWARE_CHIPSELECT

void spi_drv_init(uint8_t instance, uint8_t mode);
void spi_drv_exit(uint8_t instance);
void spi_set_baudrate(uint8_t instance, uint32_t baudrate);
void spi_write_read(uint8_t instance, const uint8_t *txData, uint16_t txSize, uint8_t *rxData, uint16_t rxSize);
void spi_write_array(uint8_t instance, const uint8_t *txData, uint16_t txSize);
void spi_read_array(uint8_t instance, uint8_t *rxData, uint16_t rxSize);
uint8_t spi_read_byte(uint8_t instance);
void spi_cs_high(uint8_t instance);
void spi_cs_low(uint8_t instance);

#endif /* HAL_SPI_HAL_H_ */
