#ifndef MODULES_SDMC_SD_BASE_H_
#define MODULES_SDMC_SD_BASE_H_

#include <stdint.h>

// SD card commands
/* GO_IDLE_STATE - init card in spi mode if CS low */
uint8_t const static CMD0 = 0x00;
/* SEND_IF_COND - verify SD Memory Card interface operating condition. */
uint8_t const static CMD8 = 0x08;
/* SEND_CSD - read the Card Specific Data (CSD register) */
uint8_t const static CMD9 = 0x09;
/* SEND_CID - read the card identification information (CID register) */
uint8_t const static CMD10 = 0x0A;
/* SEND_STATUS - read the card status register */
uint8_t const static CMD13 = 0x0D;
/* READ_BLOCK - read a single data block from the card */
uint8_t const static CMD17 = 0x11;
/* WRITE_BLOCK - write a single data block to the card */
uint8_t const static CMD24 = 0x18;
/* WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
uint8_t const static CMD25 = 0x19;
/* ERASE_WR_BLK_START - sets the address of the first block to be erased */
uint8_t const static CMD32 = 0x20;
/* ERASE_WR_BLK_END - sets the address of the last block of the continuous
  range to be erased */
uint8_t const static CMD33 = 0x21;
/* ERASE - erase all previously selected blocks */
uint8_t const static CMD38 = 0x26;
/* APP_CMD - escape for application specific command */
uint8_t const static CMD55 = 0x37;
/* READ_OCR - read the OCR register of a card */
uint8_t const static CMD58 = 0x3A;
/* SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
  pre-erased before writing */
uint8_t const static ACMD23 = 0x17;
/* SD_SEND_OP_COMD - Sends host capacity support information and
  activates the card's initialization process */
uint8_t const static ACMD41 = 0x29;

// SD status
/* status for card in the ready state */
uint8_t const static R1_READY_STATE = 0x00;
/* status for card in the idle state */
uint8_t const static R1_IDLE_STATE = 0x01;
/* status bit for illegal command */
uint8_t const static R1_ILLEGAL_COMMAND = 0x04;
/* start data token for read or write single block */
uint8_t const static DATA_START_BLOCK = 0xFE;
/* stop token for write multiple blocks*/
uint8_t const static STOP_TRAN_TOKEN = 0xFD;
/* start data token for write multiple blocks */
uint8_t const static WRITE_MULTIPLE_TOKEN = 0xFC;
/* mask for data response tokens after a write block operation */
uint8_t const static DATA_RES_MASK = 0x1F;
/* write data accepted token */
uint8_t const static DATA_RES_ACCEPTED = 0x05;

typedef struct CID {
	// byte 0
	uint8_t mid; // Manufacturer ID
	// byte 1-2
	char oid[2]; // OEM/Application ID
	// byte 3-7
	char pnm[5]; // Product name
	// byte 8
	unsigned prv_m : 4; // Product revision n.m
	unsigned prv_n : 4;
	// byte 9-12
	uint8_t psn[4]; // Product serial number
	// byte 13
	unsigned mdt_year_high : 4; // Manufacturing date
	unsigned reserved : 4;
	// byte 14
	unsigned mdt_month : 4;
	unsigned mdt_year_low : 4;
	// byte 15
	unsigned always1 : 1;
	unsigned crc : 7;
} cid_t;

// CSD for version 1.00 cards
typedef struct CSDV1 {
	// byte 0
	unsigned reserved1 : 6;
	unsigned csd_ver : 2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len : 4;
	unsigned ccc_low : 4;
	// byte 6
	unsigned c_size_high : 2;
	unsigned reserved2 : 2;
	unsigned dsr_imp : 1;
	unsigned read_blk_misalign : 1;
	unsigned write_blk_misalign : 1;
	unsigned read_bl_partial : 1;
	// byte 7
	uint8_t c_size_mid;
	// byte 8
	unsigned vdd_r_curr_max : 3;
	unsigned vdd_r_curr_min : 3;
	unsigned c_size_low : 2;
	// byte 9
	unsigned c_size_mult_high : 2;
	unsigned vdd_w_cur_max : 3;
	unsigned vdd_w_curr_min : 3;
	// byte 10
	unsigned sector_size_high : 6;
	unsigned erase_blk_en : 1;
	unsigned c_size_mult_low : 1;
	// byte 11
	unsigned wp_grp_size : 7;
	unsigned sector_size_low : 1;
	// byte 12
	unsigned write_bl_len_high : 2;
	unsigned r2w_factor : 3;
	unsigned reserved3 : 2;
	unsigned wp_grp_enable : 1;
	// byte 13
	unsigned reserved4 : 5;
	unsigned write_partial : 1;
	unsigned write_bl_len_low : 2;
	// byte 14
	unsigned reserved5: 2;
	unsigned file_format : 2;
	unsigned tmp_write_protect : 1;
	unsigned perm_write_protect : 1;
	unsigned copy : 1;
	unsigned file_format_grp : 1;
	// byte 15
	unsigned always1 : 1;
	unsigned crc : 7;
} csd1_t;

// CSD for version 2.00 cards
typedef struct CSDV2 {
	// byte 0
	unsigned reserved1 : 6;
	unsigned csd_ver : 2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len : 4;
	unsigned ccc_low : 4;
	// byte 6
	unsigned reserved2 : 4;
	unsigned dsr_imp : 1;
	unsigned read_blk_misalign : 1;
	unsigned write_blk_misalign : 1;
	unsigned read_bl_partial : 1;
	// byte 7
	unsigned reserved3 : 2;
	unsigned c_size_high : 6;
	// byte 8
	uint8_t c_size_mid;
	// byte 9
	uint8_t c_size_low;
	// byte 10
	unsigned sector_size_high : 6;
	unsigned erase_blk_en : 1;
	unsigned reserved4 : 1;
	// byte 11
	unsigned wp_grp_size : 7;
	unsigned sector_size_low : 1;
	// byte 12
	unsigned write_bl_len_high : 2;
	unsigned r2w_factor : 3;
	unsigned reserved5 : 2;
	unsigned wp_grp_enable : 1;
	// byte 13
	unsigned reserved6 : 5;
	unsigned write_partial : 1;
	unsigned write_bl_len_low : 2;
	// byte 14
	unsigned reserved7: 2;
	unsigned file_format : 2;
	unsigned tmp_write_protect : 1;
	unsigned perm_write_protect : 1;
	unsigned copy : 1;
	unsigned file_format_grp : 1;
	// byte 15
	unsigned always1 : 1;
	unsigned crc : 7;
} csd2_t;

// union of old and new style CSD register
typedef union {
	csd1_t v1;
	csd2_t v2;
} csd_t;

uint8_t sd_cmd(uint8_t cmd, uint32_t arg);
uint8_t sd_acmd(uint8_t cmd, uint32_t arg);

#endif /* MODULES_SDMC_SD_BASE_H_ */
