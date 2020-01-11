#ifndef MODULES_SDMC_SDMC_H_
#define MODULES_SDMC_SDMC_H_

#include <stdint.h>
#include "modules/sdmc/sd_base.h"

/* Protect block zero from write if nonzero */
#define SD_PROTECT_BLOCK_ZERO 1

// SD card operation timeouts
/* init timeout ms */
uint16_t const static SD_INIT_TIMEOUT = 2000;
/* erase timeout ms */
uint16_t const static SD_ERASE_TIMEOUT = 10000;
/* read timeout ms */
uint16_t const static SD_READ_TIMEOUT = 300;
/* write timeout ms */
uint16_t const static SD_WRITE_TIMEOUT = 600;

// SD card errors
/* timeout error for command CMD0 */
uint8_t const static SD_CARD_ERROR_CMD0 = 0x1;
/* CMD8 was not accepted - not a valid SD card */
uint8_t const static SD_CARD_ERROR_CMD8 = 0x2;
/* card returned an error response for CMD17 (read block) */
uint8_t const static SD_CARD_ERROR_CMD17 = 0x3;
/* card returned an error response for CMD24 (write block) */
uint8_t const static SD_CARD_ERROR_CMD24 = 0x4;
/*  WRITE_MULTIPLE_BLOCKS command failed */
uint8_t const static SD_CARD_ERROR_CMD25 = 0x05;
/* card returned an error response for CMD58 (read OCR) */
uint8_t const static SD_CARD_ERROR_CMD58 = 0x06;
/* SET_WR_BLK_ERASE_COUNT failed */
uint8_t const static SD_CARD_ERROR_ACMD23 = 0x07;
/* card's ACMD41 initialization process timeout */
uint8_t const static SD_CARD_ERROR_ACMD41 = 0x08;
/* card returned a bad CSR version field */
uint8_t const static SD_CARD_ERROR_BAD_CSD = 0x09;
/* erase block group command failed */
uint8_t const static SD_CARD_ERROR_ERASE = 0x0A;
/* card not capable of single block erase */
uint8_t const static SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0x0B;
/* Erase sequence timed out */
uint8_t const static SD_CARD_ERROR_ERASE_TIMEOUT = 0x0C;
/* card returned an error token instead of read data */
uint8_t const static SD_CARD_ERROR_READ = 0x0D;
/* read CID or CSD failed */
uint8_t const static SD_CARD_ERROR_READ_REG = 0x0E;
/* timeout while waiting for start of read data */
uint8_t const static SD_CARD_ERROR_READ_TIMEOUT = 0x0F;
/* card did not accept STOP_TRAN_TOKEN */
uint8_t const static SD_CARD_ERROR_STOP_TRAN = 0x10;
/* card returned an error token as a response to a write operation */
uint8_t const static SD_CARD_ERROR_WRITE = 0x11;
/* attempt to write protected block zero */
uint8_t const static SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0x12;
/* card did not go ready for a multiple block write */
uint8_t const static SD_CARD_ERROR_WRITE_MULTIPLE = 0x13;
/* card returned an error to a CMD13 status check after a write */
uint8_t const static SD_CARD_ERROR_WRITE_PROGRAMMING = 0x14;
/* timeout occurred during write programming */
uint8_t const static SD_CARD_ERROR_WRITE_TIMEOUT = 0x15;
/* incorrect rate selected */
uint8_t const static SD_CARD_ERROR_SCK_RATE = 0x16;

// card types
/* Standard capacity V1 SD card */
uint8_t const static SD_CARD_TYPE_SD1 = 1;
/* Standard capacity V2 SD card */
uint8_t const static SD_CARD_TYPE_SD2 = 2;
/* High Capacity SD card */
uint8_t const static SD_CARD_TYPE_SDHC = 3;

uint8_t const static SD_STATUS_ERROR = 1;
uint8_t const static SD_STATUS_OK = 0;

uint8_t sdmc_type;

uint8_t sdmc_init(void);
uint32_t sdmc_get_blocks(void);
void sdmc_set_type(uint8_t type);
uint8_t sdmc_get_type(void);
void sdmc_set_error(uint8_t code);
uint8_t sdmc_get_error(void);
uint8_t sdmc_get_status(void);
void sdmc_set_pbr(uint8_t enable); // partial block read
uint8_t sdmc_get_pbr(void);

uint8_t sdmc_wait_start_block(void);
uint8_t sdmc_read_reg(uint8_t cmd, uint8_t* data);
uint8_t sdmc_rdcid(cid_t* cid);
uint8_t sdmc_rdcsd(csd_t* csd);
uint8_t sdmc_rdblk(uint32_t block, uint8_t* data);
uint8_t sdmc_rddata(uint32_t block,
		uint16_t offset, uint16_t count, uint8_t* data);
void sdmc_rdend(void);

uint8_t sdmc_get_erase_block(void);
uint8_t sdmc_erase(uint32_t firstBlock, uint32_t lastBlock);
uint8_t sdmc_wait_idle(uint16_t timeout_ms);
uint8_t sdmc_wrblk(uint32_t block, const uint8_t* data, uint8_t blocking);
uint8_t sdmc_wrdata(uint8_t token, const uint8_t* data);

uint8_t sdmc_is_busy(void);

#endif /* MODULES_SDMC_SDMC_H_ */
