#include "HAL/spi_hal.h"

#include <stdlib.h>
#include "lpspi_master_driver.h"
#include "lpspi_slave_driver.h"
#include "SPI0.h"

#define SPI_INSTANCE_NUMBER		1

void spi_mapping_handler(uint8_t instance);

lpspi_state_t * spi_state;
lpspi_master_config_t * spi_master_config;
lpspi_slave_config_t * spi_slave_config;
uint32_t spi_cs_pin;

static uint8_t spi_mode[SPI_INSTANCE_NUMBER];

void spi_drv_init(uint8_t instance, uint8_t mode) {
	spi_mapping_handler(instance);
	spi_mode[instance] = mode;

	switch(mode) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterInit(instance, spi_state, spi_master_config);
		break;
	case SPI_SLAVE_MODE:
		LPSPI_DRV_SlaveInit(instance, spi_state, spi_slave_config);
		break;
	default:
		break;
	}
}

void spi_drv_exit(uint8_t instance) {
	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterDeinit(instance);
		break;
	case SPI_SLAVE_MODE:
		LPSPI_DRV_SlaveDeinit(instance);
		break;
	default:
		break;
	}
}

void spi_set_baudrate(uint8_t instance, uint32_t baudrate) {
	spi_mapping_handler(instance);
	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		spi_master_config->bitsPerSec = baudrate;
		LPSPI_DRV_MasterConfigureBus(instance, spi_master_config, NULL);
		break;
	}
}

void spi_write_read(uint8_t instance, const uint8_t *txData, uint16_t txSize, uint8_t *rxData, uint16_t rxSize) {
	uint8_t *rxBuf = calloc(txSize, sizeof(uint8_t));
	uint8_t *txBuf = calloc(rxSize, sizeof(uint8_t));
	uint16_t i;

	for(i = 0; i < txSize; i++) {
		*(rxBuf + i) = 0xFF;
	}

	for(i = 0; i < rxSize; i++) {
		*(txBuf + i) = 0xFF;
	}

	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterTransferBlocking(instance, txData, rxBuf, txSize, 200);
		LPSPI_DRV_MasterTransferBlocking(instance, txBuf, rxData, rxSize, 200);
		break;
	}
	free(rxBuf);
	free(txBuf);
}

void spi_write_array(uint8_t instance, const uint8_t *txData, uint16_t txSize) {
	uint8_t *rxBuf = calloc(txSize, sizeof(uint8_t));

	for(uint16_t i = 0; i < txSize; i++) {
		*(rxBuf + i) = 0xFF;
	}

	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterTransferBlocking(instance,  txData, rxBuf, txSize, 200);
		break;
	case SPI_SLAVE_MODE:
		LPSPI_DRV_SlaveTransferBlocking(instance,  txData, rxBuf, txSize, 200);
		break;
	default:
		break;
	}
	free(rxBuf);
}

void spi_read_array(uint8_t instance, uint8_t *rxData, uint16_t rxSize) {
	uint8_t *txBuf = calloc(rxSize, sizeof(uint8_t));

	for(uint16_t i = 0; i < rxSize; i++) {
		*(txBuf + i) = 0xFF;
	}

	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterTransferBlocking(instance,  txBuf, rxData, rxSize, 200);
		break;
	case SPI_SLAVE_MODE:
		LPSPI_DRV_SlaveTransferBlocking(instance,  txBuf, rxData, rxSize, 200);
		break;
	default:
		break;
	}
	free(txBuf);
}

uint8_t spi_read_byte(uint8_t instance) {
	uint8_t txBuf[1] = {0xFF};
	uint8_t rxBuf[1] = {0xFF};

	switch(spi_mode[instance]) {
	case SPI_MASTER_MODE:
		LPSPI_DRV_MasterTransferBlocking(instance, txBuf, rxBuf, 1, 200);
		break;
	case SPI_SLAVE_MODE:
		LPSPI_DRV_SlaveTransferBlocking(instance, txBuf, rxBuf, 1, 200);
		break;
	default:
		break;
	}
	return rxBuf[0];
}

#ifndef SPI_HARDWARE_CHIPSELECT
#include "HAL/gpio_hal.h"
#endif

void spi_cs_high(uint8_t instance) {
#ifndef SPI_HARDWARE_CHIPSELECT
	spi_mapping_handler(instance);
	gpio_set_level(spi_cs_pin, GPIO_LEVEL_HIGH);
#endif
}

void spi_cs_low(uint8_t instance) {
#ifndef SPI_HARDWARE_CHIPSELECT
	spi_mapping_handler(instance);
	gpio_set_level(spi_cs_pin, GPIO_LEVEL_LOW);
#endif
}

inline void spi_mapping_handler(uint8_t instance) {
	switch(instance) {
	case 0:
		spi_cs_pin = 37; // Port B-5
		spi_state = &SPI0State;
		spi_master_config = &SPI0_MasterConfig0;
		spi_slave_config = &SPI0_SlaveConfig0;
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}
