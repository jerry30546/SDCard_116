#include "modules/sdmc/sdmc.h"
#include "modules/time/time.h"

#include "HAL/spi_hal.h"

#include <stdlib.h>
#include <string.h>

#define SDMC_SPI_PORT	0

uint8_t status; // SD status
uint8_t is_reading; // reading block data
uint8_t sdmc_error; // Stored SDMC error code
uint8_t is_pbr; // Partial block read permission
uint32_t block_index; // Index of block to be read
uint16_t offset_index; // Index of byte in block to be read

/**************************************************************************
 * Function Name:	sd_cmd
 * Description:		Send SD CMD via SPI
 * @ param1 cmd:	SD CMD
 * @ param2 arg:	Argument of SD CMD
 * Returns: 		Bus status, 0 - success, otherwise error occurred
 **************************************************************************/
uint8_t sd_cmd(uint8_t cmd, uint32_t arg) {
	uint8_t data[6];
	uint8_t crc = 0x00;
	if (cmd == CMD0) {
		crc = 0x95;
	} else if (cmd == CMD8) {
		crc = 0x87;
	}

	data[0] = cmd | 0x40;
	data[1] = (uint8_t)(arg >> 24);
	data[2] = (uint8_t)(arg >> 16);
	data[3] = (uint8_t)(arg >> 8);
	data[4] = (uint8_t)(arg >> 0);
	data[5] = crc;

	sdmc_rdend();

	sdmc_wait_idle(300);

	spi_cs_low(SDMC_SPI_PORT);

	spi_write_array(SDMC_SPI_PORT, data, 6);
	uint8_t i;
	for(i = 0, status = 0xFF; (status & 0x80) && (i != 0xFF); i++) {
		status = spi_read_byte(SDMC_SPI_PORT);
	}

	spi_cs_high(SDMC_SPI_PORT);
	return status;
}

/**************************************************************************
 * Function Name:	sd_acmd
 * Description:		Send SD ACMD via SPI
 * @ param1 cmd:	SD ACMD
 * @ param2 arg:	Argument of SD ACMD
 * Returns: 		Bus status, 0 - success, otherwise error occurred
 **************************************************************************/
uint8_t sd_acmd(uint8_t cmd, uint32_t arg) {
	sd_cmd(CMD55, 0);
	return sd_cmd(cmd, arg);
}

/**************************************************************************
 * Function Name:	sdmc_init
 * Description:		Initial SDMC in SPI mode and get SDMC type
 * Returns: 		SDMC status, 0 - success, 1 - error occurred
 **************************************************************************/
uint8_t sdmc_init(void) {
	uint8_t data[10] = {[0 ... 9] = 0xFF};
	uint32_t arg;

	is_reading = 0;
	is_pbr = 0;
	sdmc_error = 0;
	sdmc_type = 0;

	// Reset SD Card
	// must supply at least 74 clock cycles with CS high.
	// clock rate need less than 400kHz
	spi_set_baudrate(SDMC_SPI_PORT, 250000);
	spi_cs_high(SDMC_SPI_PORT);
	spi_write_array(SDMC_SPI_PORT, data, 10); // 80 clocks

	uint32_t time = millis();
	// Set SD Card in SPI mode
	while (sd_cmd(CMD0, 0) != R1_IDLE_STATE) {
		if ((millis() - time) > SD_INIT_TIMEOUT) {
			sdmc_set_error(SD_CARD_ERROR_CMD0);
			return SD_STATUS_ERROR;
		}
	}

	// check SD version
	if ((sd_cmd(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
		sdmc_set_type(SD_CARD_TYPE_SD1);
	} else {
		// only need last byte of r7 response
		spi_cs_low(SDMC_SPI_PORT);
		for (uint8_t i = 0; i < 4; i++) {
			status = spi_read_byte(SDMC_SPI_PORT);
		}
		spi_cs_high(SDMC_SPI_PORT);
		if (status != 0xAA) {
			sdmc_set_error(SD_CARD_ERROR_CMD8);
			return SD_STATUS_ERROR;
		}
		sdmc_set_type(SD_CARD_TYPE_SD2);
	}
	arg = (sdmc_get_type() == SD_CARD_TYPE_SD2) ? 0x50000000 : 0;

	time = millis();
	while (sd_acmd(ACMD41, arg) != R1_READY_STATE) {
		if ((millis() - time) > SD_INIT_TIMEOUT) {
			sdmc_set_error(SD_CARD_ERROR_ACMD41);
			return SD_STATUS_ERROR;
		}
	}

	// if SD2 read OCR register to check for SDHC card
	if (sdmc_get_type() == SD_CARD_TYPE_SD2) {
		if (sd_cmd(CMD58, 0)) {
			sdmc_set_error(SD_CARD_ERROR_CMD58);
			return SD_STATUS_ERROR;
		}
		spi_cs_low(SDMC_SPI_PORT);
		if (spi_read_byte(SDMC_SPI_PORT) == 0xC0) {
			sdmc_set_type(SD_CARD_TYPE_SDHC);
		}
		// discard reset of OCR - contains allowed voltage range
		for (uint8_t i = 0; i < 3; i++) {
			spi_read_byte(SDMC_SPI_PORT);
		}
		spi_cs_high(SDMC_SPI_PORT);
	}

	spi_set_baudrate(SDMC_SPI_PORT, 1000000);
	return SD_STATUS_OK;

}

/**************************************************************************
 * Function Name:	sdmc_get_blocks
 * Description:		Retrieve SDMC block size
 * Returns:			size of SDMC block
 **************************************************************************/
uint32_t sdmc_get_blocks(void) {
	csd_t csd;

	if (sdmc_rdcsd(&csd)) {
		return 0;
	}

	if (csd.v1.csd_ver == 0) {
		uint8_t read_bl_len = csd.v1.read_bl_len;
		uint16_t c_size = (csd.v1.c_size_high << 10)
						  | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
		uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1)
							  | csd.v1.c_size_mult_low;
		return (uint32_t)((c_size + 1) << (c_size_mult + read_bl_len - 7));
	} else if (csd.v2.csd_ver == 1) {
		uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16)
						  | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
		return (c_size + 1) << 10;
	} else {
		sdmc_set_error(SD_CARD_ERROR_BAD_CSD);
		return 0;
	}
}

/**************************************************************************
 * Function Name:	sdmc_set_type
 * Description:		Store SDMC type
 * @ param1 type:	SDMC type ID
 **************************************************************************/
void sdmc_set_type(uint8_t type) {
	sdmc_type = type;
}

/**************************************************************************
 * Function Name:	sdmc_get_type
 * Description:		Get stored SDMC type
 * Returns: 		SDMC type ID
 **************************************************************************/
uint8_t sdmc_get_type(void) {
	return sdmc_type;
}

/**************************************************************************
 * Function Name:	sdmc_set_error
 * Description:		Store SDMC operation error code
 * @ param1 code:	SDMC error code
 **************************************************************************/
void sdmc_set_error(uint8_t code) {
	sdmc_error = code;
}

/**************************************************************************
 * Function Name:	sdmc_get_error
 * Description:		Get stored SDMC operation error code
 * Returns: 		SDMC operation error code
 **************************************************************************/
uint8_t sdmc_get_error(void) {
	return sdmc_error;
}

/**************************************************************************
 * Function Name:	sdmc_get_status
 * Description:		Get SDMC status
 * Returns: 		SDMC status
 **************************************************************************/
uint8_t sdmc_get_status(void) {
	return status;
}

/**************************************************************************
 * Function Name:	sdmc_set_pbr
 * Description:		Set partial block reads permission
 * @ param1 enable:	0 - disable, 1 - enable
 **************************************************************************/
void sdmc_set_pbr(uint8_t enable) {
	sdmc_rdend();
	is_pbr = enable;
}

/**************************************************************************
 * Function Name:	sdmc_get_pbr
 * Description:		Get partial block read permission
 * Returns: 		PBR permission, 0 - disable, 1 - enable
 **************************************************************************/
uint8_t sdmc_get_pbr(void) {
	return is_pbr;
}

/**************************************************************************
 * Function Name:	sdmc_wait_start_block
 * Description:		Wait for start block token from SDMC
 * Returns: 		SDMC status, 0 - success, 1 - error occurred
 **************************************************************************/
uint8_t sdmc_wait_start_block(void) {
	uint32_t time = millis();
	spi_cs_low(SDMC_SPI_PORT);
	while ((status = spi_read_byte(SDMC_SPI_PORT)) == 0xFF) {

		if ((millis() - time) > SD_READ_TIMEOUT) {
			sdmc_set_error(SD_CARD_ERROR_READ_TIMEOUT);
			spi_cs_high(SDMC_SPI_PORT);
			return SD_STATUS_ERROR;
		}
	}
	spi_cs_high(SDMC_SPI_PORT);

	if (status != DATA_START_BLOCK) {
		sdmc_set_error(SD_CARD_ERROR_READ);
		return SD_STATUS_ERROR;
	}
	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:	sdmc_read_reg
 * Description:		Read SDMC register value from SPI
 * @ param1 cmd:	SD CMD
 * @ param2 data:	Data pointer, pass data container
 * Returns: 		SDMC status, 0 - success, 1 - error occurred
 **************************************************************************/
uint8_t sdmc_read_reg(uint8_t cmd, uint8_t* data) {
	uint8_t crc[2];

	if (sd_cmd(cmd, 0)) {
		sdmc_set_error(SD_CARD_ERROR_READ_REG);
		return SD_STATUS_ERROR;
	}
	if (sdmc_wait_start_block()) {
		return SD_STATUS_ERROR;
	}

	spi_cs_low(SDMC_SPI_PORT);
	spi_read_array(SDMC_SPI_PORT, data, 16);
	spi_read_array(SDMC_SPI_PORT, crc, 2);
	spi_cs_high(SDMC_SPI_PORT);
	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:	sdmc_rdcid
 * Description:		Read SDMC CID
 * @ param1 cid:	CID data container pointer
 * Returns: 		SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_rdcid(cid_t* cid) {
	return sdmc_read_reg(CMD10, (uint8_t*)cid);
}

/**************************************************************************
 * Function Name:	sdmc_rdcsd
 * Description:		Read SDMC CSD
 * @ param1 csd:	CSD data container pointer
 * Returns: 		SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_rdcsd(csd_t* csd) {
	return sdmc_read_reg(CMD9, (uint8_t*)csd);
}

/**************************************************************************
 * Function Name:	sdmc_rdblk
 * Description:		Read 1 of SDMC block data
 * @ param1 block:	Index of block to read
 * @ param2 data:	Data container
 * Returns: 		SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_rdblk(uint32_t block, uint8_t* data) {
	return sdmc_rddata(block, 0, 512, data);
}

/**************************************************************************
 * Function Name:	sdmc_rddata
 * Description:		Read data from SDMC block
 * @ param1 block:	Index of block to read
 * @ param2 offset:	Number of bytes to skip at start of block
 * @ param3 count:	Number of bytes to read
 * @ param4 data:	Data container
 * Returns: 		SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_rddata(uint32_t block,
		uint16_t offset, uint16_t count, uint8_t* data) {

	if (count == 0) {
		return SD_STATUS_OK;
	}

	if ((count + offset) > 512) {
		return SD_STATUS_ERROR;
	}

	if (!is_reading || block != block_index || offset < offset_index) {
		block_index = block;
		// use address if not SDHC card
		if (sdmc_get_type() != SD_CARD_TYPE_SDHC) {
			block <<= 9;
		}

		if (sd_cmd(CMD17, block)) {
			sdmc_set_error(SD_CARD_ERROR_CMD17);
			return SD_STATUS_ERROR;
		}

		if (sdmc_wait_start_block()) {
			return SD_STATUS_ERROR;
		}

		offset_index = 0;
		is_reading = 1;
	}

	// skip data before offset
	spi_cs_low(SDMC_SPI_PORT);
	for (; offset_index < offset; offset_index++) {
		spi_read_byte(SDMC_SPI_PORT);
	}

	// transfer data
	spi_read_array(SDMC_SPI_PORT, data, count);
	spi_cs_high(SDMC_SPI_PORT);

	offset_index += count;

	if (!is_pbr || offset_index >= 512) {
		// read rest of data, checksum and set chip select high
		sdmc_rdend();
	}

	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:	sdmc_rdend
 * Description:		Skip remaining block data and CRCs in Non-PBR mode
 **************************************************************************/
void sdmc_rdend(void) {
	if (is_reading) {
		// skip data and 2 bytes CRC
		while (offset_index++ < 514) {
			spi_read_byte(SDMC_SPI_PORT);
		}

		is_reading = 0;
	}
}

/**************************************************************************
 * Function Name:	sdmc_get_erase_block
 * Description:		Determine if SDMC supports single block erase
 * Returns: 		SDMC support, 0 - Unsupported, 1 - Supported
 **************************************************************************/
uint8_t sdmc_get_erase_block(void) {
	csd_t csd;
	return sdmc_rdcsd(&csd) ? 0 : csd.v1.erase_blk_en;
}

/**************************************************************************
 * Function Name:		sdmc_erase
 * Description:			Erase SDMC block data
 * @ param1 firstBlock:	Index of first block for erasing data
 * @ param2 lastBlock:	Index of last block for erasing data
 * Returns: 			SDMC status, 0 - success, 1 - error occurred
 **************************************************************************/
uint8_t sdmc_erase(uint32_t firstBlock, uint32_t lastBlock) {
	if (!sdmc_get_erase_block()) {
		sdmc_set_error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
		return SD_STATUS_ERROR;
	}

	if (sdmc_get_type() != SD_CARD_TYPE_SDHC) {
		firstBlock <<= 9;
		lastBlock <<= 9;
	}

	if (sd_cmd(CMD32, firstBlock)
			|| sd_cmd(CMD33, lastBlock)
			|| sd_cmd(CMD38, 0)) {
		sdmc_set_error(SD_CARD_ERROR_ERASE);
		return SD_STATUS_ERROR;
	}

	if (sdmc_wait_idle(SD_ERASE_TIMEOUT)) {
		sdmc_set_error(SD_CARD_ERROR_ERASE_TIMEOUT);
		return SD_STATUS_ERROR;
	}

	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:		sdmc_wait_idle
 * Description:			Wait for SDMC idle
 * @ param1 timeout_ms:	Timeout limitation, unit millisecond
 * Returns:				Bus status, 0 - idle, 1 - busy
 **************************************************************************/
uint8_t sdmc_wait_idle(uint16_t timeout_ms) {
	spi_cs_low(SDMC_SPI_PORT);
	uint32_t time = millis();
	do {
		if (spi_read_byte(SDMC_SPI_PORT) == 0xFF) {
			spi_cs_high(SDMC_SPI_PORT);
			return SD_STATUS_OK;
		}
	} while ((millis() - time) < timeout_ms);
	spi_cs_high(SDMC_SPI_PORT);
	return SD_STATUS_ERROR;
}

/**************************************************************************
 * Function Name:		sdmc_wrblk
 * Description:			Writes a 512 byte block to an SD card
 * @ param1 block:		Index of block to write
 * @ param2 data:		Pointer to the location of the data to be written
 * @ param3 blocking:	Write in blocking or non-blocking method
 * Returns: 			SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_wrblk(uint32_t block,
		const uint8_t* data, uint8_t blocking) {

#if SD_PROTECT_BLOCK_ZERO
	// don't allow write to first block
	if (block == 0) {
		sdmc_set_error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
		return SD_STATUS_ERROR;
	}
#endif  // SD_PROTECT_BLOCK_ZERO

	// use address if not SDHC card
	if (sdmc_get_type() != SD_CARD_TYPE_SDHC) {
		block <<= 9;
	}

	if (sd_cmd(CMD24, block)) {
		sdmc_set_error(SD_CARD_ERROR_CMD24);
		return SD_STATUS_ERROR;
	}

	if (sdmc_wrdata(DATA_START_BLOCK, data)) {
		return SD_STATUS_ERROR;
	}
	if (blocking) {
		// wait for flash programming to complete
		if (sdmc_wait_idle(SD_WRITE_TIMEOUT)) {
			sdmc_set_error(SD_CARD_ERROR_WRITE_TIMEOUT);
			return SD_STATUS_ERROR;
		}
		// response is r2 so get and check two bytes for nonzero
		status = sd_cmd(CMD13, 0);
		spi_cs_low(SDMC_SPI_PORT);
		status |= spi_read_byte(SDMC_SPI_PORT);
		spi_cs_high(SDMC_SPI_PORT);
		if (status) {
			sdmc_set_error(SD_CARD_ERROR_WRITE_PROGRAMMING);
			return SD_STATUS_ERROR;
		}
	}

	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:	sdmc_wrdata
 * Description:		Send one block of data for write block
 * @ param1 token:	SDMC write block token
 * @ param2 data:	Pointer to the location of the data to be written
 * Returns: 		SDMC status, 0 - success, 1 - occurred error
 **************************************************************************/
uint8_t sdmc_wrdata(uint8_t token, const uint8_t* data) {
	uint8_t crc[2] = {0xFF, 0xFF};

	spi_cs_low(SDMC_SPI_PORT);
	spi_write_array(SDMC_SPI_PORT, (uint8_t*)&token, 1);
	spi_write_array(SDMC_SPI_PORT, data, 512);
	spi_write_array(SDMC_SPI_PORT, crc, 2);
	status = spi_read_byte(SDMC_SPI_PORT);
	spi_cs_high(SDMC_SPI_PORT);

	if ((status & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
		sdmc_set_error(SD_CARD_ERROR_WRITE);
		return SD_STATUS_ERROR;
	}
	return SD_STATUS_OK;
}

/**************************************************************************
 * Function Name:	sdmc_is_busy
 * Description:		Check SDMC whether in busy
 * Returns: 		SDMC status, 0 - idle, 1 - busy
 **************************************************************************/
uint8_t sdmc_is_busy(void) {
	spi_cs_low(SDMC_SPI_PORT);
	uint8_t b = spi_read_byte(SDMC_SPI_PORT);
	spi_cs_high(SDMC_SPI_PORT);

	return (b != 0xFF);
}
