#ifndef MMC_H
#define MMC_H

// http://elm-chan.org/docs/mmc/mmc_e.html
// https://www.fatalerrors.org/a/sd-card-initialization-and-command-details.html
// https://yannik520.github.io/sdio.html
// http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf
// http://problemkaputt.de/gbatek-dsi-sd-mmc-protocol-and-i-o-ports.htm

#include <stdint.h>

#define MMC_CRC_POLY 0x09
#define MMC_GO_IDLE_STATE_CRC_BYTE 0x95

typedef enum
{
	MMC_CMD_GO_IDLE_STATE           =  0,
	MMC_CMD_GO_PRE_IDLE_STATE       =  0,
	MMC_CMD_GO_BOOT_INITIATION      =  0,
	MMC_CMD_SEND_OP_COND            =  1,
	MMC_CMD_ALL_SEND_CID            =  2,
	MMC_CMD_SEND_RELATIVE_ADDR      =  3,
	MMC_CMD_SEND_SET_DSR            =  4,
	MMC_CMD_SLEEP_AWAKE             =  5,
	MMC_CMD_IO_SEND_OP_COND         =  5,
	MMC_CMD_SWITCH                  =  6,
	MMC_CMD_SWITCH_FUNC             =  6,
	MMC_CMD_SELECT_CARD             =  7,
	MMC_CMD_SEND_IF_COND            =  8,
	MMC_CMD_SEND_EXT_CSD            =  8,
	MMC_CMD_SEND_CSD                =  9,
	MMC_CMD_SEND_CID                = 10,
	MMC_CMD_VOLTAGE_SWITCH          = 11,
	MMC_CMD_READ_DAT_UNTIL_STOP     = 11,
	MMC_CMD_STOP_TRANSMISSION       = 12,
	MMC_CMD_SEND_STATUS             = 13,
	MMC_CMD_BUSTEST_R               = 14,
	MMC_CMD_GO_INACTIVE_STATE       = 15,
	MMC_CMD_SET_BLOCKLEN            = 16,
	MMC_CMD_READ_SINGLE_BLOCK       = 17,
	MMC_CMD_READ_MULTIPLE_BLOCK     = 18,
	MMC_CMD_SET_TUNING_BLOCK        = 19,
	MMC_CMD_BUSTEST_W               = 19,
	MMC_CMD_SPEED_CLASS_CONTROL     = 20,
	MMC_CMD_WRITE_DAT_UNTIL_STOP    = 20,
	MMC_CMD_SET_BLOCK_COUNT         = 23,
	MMC_CMD_WRITE_BLOCK             = 24,
	MMC_CMD_WRITE_MULTIPLE_BLOCK    = 25,
	MMC_CMD_PROGRAM_CID             = 26,
	MMC_CMD_PROGRAM_CSD             = 27,
	MMC_CMD_SET_WRITE_PROT          = 28,
	MMC_CMD_CLR_WRITE_PROT          = 29,
	MMC_CMD_SEND_WRITE_PROT         = 30,
	MMC_CMD_SEND_WRITE_PROT_TYPE    = 31,
	MMC_CMD_ERASE_WR_BLK_START_ADDR = 32,
	MMC_CMD_ERASE_WR_BLK_END_ADDR   = 33,
	MMC_CMD_ERASE_GROUP_START       = 35,
	MMC_CMD_ERASE_GROUP_END         = 36,
	MMC_CMD_ERASE                   = 38,
	MMC_CMD_FAST_IO                 = 39,
	MMC_CMD_GO_IRQ_STATE            = 40,
	MMC_CMD_LOCK_UNLOCK             = 42,
	MMC_CMD_READ_EXTR_SINGLE        = 48,
	MMC_CMD_WRITE_EXTR_SINGLE       = 49,
	MMC_CMD_IO_RW_DIRECT            = 52,
	MMC_CMD_IO_RW_EXTENDED          = 53,
	MMC_CMD_APP_CMD                 = 55,
	MMC_CMD_GEN_CMD                 = 56,
	MMC_CMD_READ_OCR                = 58,
	MMC_CMD_READ_EXTR_MULTI         = 58,
	MMC_CMD_CRC_ON_OFF              = 59,
	MMC_CMD_WRITE_EXTR_MULTI        = 59,
} mmc_cmd_e;

typedef enum
{
	MMC_CMD_APP_SET_BUS_WIDTH            =  6,
	MMC_CMD_APP_SD_STATUS                = 13,
	MMC_CMD_APP_SECURE_READ_MULTI_BLOCK  = 18,
	MMC_CMD_APP_SEND_NUM_WR_BLOCKS       = 22,
	MMC_CMD_APP_SET_WR_BLK_ERASE_COUNT   = 23,
	MMC_CMD_APP_SECURE_WRITE_MULTI_BLOCK = 25,
	MMC_CMD_APP_SECURE_WRITE_MKB         = 26,
	MMC_CMD_APP_SECURE_ERASE             = 38,
	MMC_CMD_APP_SEND_OP_COND             = 41,
	MMC_CMD_APP_SET_CLR_CARD_DETECT      = 42,
	MMC_CMD_APP_GET_MKB                  = 43,
	MMC_CMD_APP_GET_MID                  = 44,
	MMC_CMD_APP_SET_CER_RN1              = 45,
	MMC_CMD_APP_SET_CER_RN2              = 46,
	MMC_CMD_APP_SET_CER_RES1             = 47,
	MMC_CMD_APP_SET_CER_RES2             = 48,
	MMC_CMD_APP_CHANGE_SECURE_AREA       = 49,
	MMC_CMD_APP_SEND_SCR                 = 51,
} mmc_cmd_app_e;

typedef enum
{
	MMC_DATA_TOKEN_WRITE_MULTIPLE    = 0xFC,
	MMC_DATA_TOKEN_STOP_TRANSMISSION = 0xFD,
	MMC_DATA_TOKEN                   = 0xFE,
	MMC_DATA_TOKEN_NONE              = 0xFF,
} mmc_data_token_e;

typedef enum
{
	MMC_DATA_RESPONSE_ACCEPTED    = 0b0010,
	MMC_DATA_RESPONSE_CRC_ERROR   = 0b0101,
	MMC_DATA_RESPONSE_WRITE_ERROR = 0b0110,
} mmc_data_response_e;

typedef union
{
	struct
	{
		uint8_t start_bit : 1;
		uint8_t response  : 4;
		uint8_t dont_care : 3;
	};

	uint8_t mask;
} mmc_data_response_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t  index;
	uint32_t argument;
	uint8_t  crc;
} mmc_cmd_frame_t;

typedef union
{
	struct
	{
		uint8_t in_idle_state        : 1;
		uint8_t erase_reset          : 1;
		uint8_t illegal_command      : 1;
		uint8_t crc_error            : 1;
		uint8_t erase_sequence_error : 1;
		uint8_t address_error        : 1;
		uint8_t parameter_error      : 1;
		uint8_t start_bit            : 1;
	};

	uint8_t mask;
} mmc_resp_r1_t;

typedef union
{
	struct
	{
		uint32_t check_pattern   :  8;
		uint32_t voltage         :  4;
		uint32_t reserved        : 16;
		uint32_t command_version :  4;
	};

	uint32_t mask;
} mmc_send_if_cond_t;

typedef union
{
	struct
	{
		uint32_t reserved29_0 : 30;
		uint32_t hcs          :  1;
		uint32_t reserved31   :  1;
	};

	uint32_t mask;
} mmc_send_op_cond_t;

typedef union
{
	struct
	{
		uint32_t reserved7_0   : 15;
		uint32_t vdd_2v7_2v8   :  1;
		uint32_t vdd_2v8_2v9   :  1;
		uint32_t vdd_2v9_3v0   :  1;
		uint32_t vdd_3v0_3v1   :  1;
		uint32_t vdd_3v1_3v2   :  1;
		uint32_t vdd_3v2_3v3   :  1;
		uint32_t vdd_3v3_3v4   :  1;
		uint32_t vdd_3v4_3v5   :  1;
		uint32_t vdd_3v5_3v6   :  1;
		uint32_t s18a          :  1;
		uint32_t reserved28_25 :  4;
		uint32_t uhs2          :  1;
		uint32_t ccs           :  1;
		uint32_t ready         :  1;
	};

	uint32_t mask;
} mmc_ocr_t;

typedef struct
{
	uint8_t reserved121_120    :  2;
	uint8_t spec_vers          :  4;
	uint8_t csd_structure      :  2;

	uint8_t taac               :  8;

	uint8_t nsac               :  8;

	uint8_t tran_speed         :  8;

	uint8_t ccc_11_4           :  8;

	uint8_t read_bl_len        :  4;
	uint8_t ccc_3_0            :  4;

	union
	{
		struct
		{
			uint8_t c_size_11_10       : 2;
			uint8_t reserved75_74      : 2;
			uint8_t dsr_imp            : 1;
			uint8_t read_blk_misalign  : 1;
			uint8_t write_blk_misalign : 1;
			uint8_t read_bl_partial    : 1;

			uint8_t c_size_9_2         : 8;

			uint8_t vdd_r_curr_max     : 3;
			uint8_t vdd_r_curr_min     : 3;
			uint8_t c_size_1_0         : 2;

			uint8_t c_size_mult_2_1    : 2;
			uint8_t vdd_w_curr_max     : 3;
			uint8_t vdd_w_curr_min     : 3;

			uint8_t sector_size        : 6;
			uint8_t erase_blk_en       : 1;
			uint8_t c_size_mult_0      : 1;

			uint8_t wp_grp_size        : 7;
			uint8_t sector_size_0      : 1;
		};

		struct
		{
			uint8_t c_size_11_10       : 2;
			uint8_t reserved75_74      : 2;
			uint8_t dsr_imp            : 1;
			uint8_t read_blk_misalign  : 1;
			uint8_t write_blk_misalign : 1;
			uint8_t read_bl_partial    : 1;

			uint8_t c_size_9_2         : 8;

			uint8_t vdd_r_curr_max     : 3;
			uint8_t vdd_r_curr_min     : 3;
			uint8_t c_size_1_0         : 2;

			uint8_t c_size_mult_2_1    : 2;
			uint8_t vdd_w_curr_max     : 3;
			uint8_t vdd_w_curr_min     : 3;

			uint8_t erase_grp_mult_4_3 : 2;
			uint8_t erase_grp_size     : 5;
			uint8_t c_size_mult_0      : 1;

			uint8_t wp_grp_size        : 5;
			uint8_t erase_grp_mult_2_0 : 3;
		} mmc;

		struct
		{

			uint8_t reserved75_72      : 4;
			uint8_t dsr_imp            : 1;
			uint8_t read_blk_misalign  : 1;
			uint8_t write_blk_misalign : 1;
			uint8_t read_bl_partial    : 1;

			uint8_t c_size_21_16       : 6;
			uint8_t reserved71_70      : 2;

			uint8_t c_size_15_8        : 8;

			uint8_t c_size_7_0         : 8;

			uint8_t sector_size        : 6;
			uint8_t erase_blk_en       : 1;
			uint8_t reserved47         : 1;

			uint8_t wp_grp_size        : 7;
			uint8_t sector_size_0      : 1;
		} v2;
	};

	uint8_t write_bl_len_3_2   : 2;
	uint8_t r2w_factor         : 3;
	uint8_t default_ecc        : 2;
	uint8_t wp_grp_enable      : 1;

	uint8_t content_prop_app   : 1;
	uint8_t reserved20_17      : 4;
	uint8_t write_bl_partial   : 1;
	uint8_t write_bl_len_1_0   : 2;

	uint8_t ecc                : 2;
	uint8_t file_format        : 2;
	uint8_t temp_write_protect : 1;
	uint8_t perm_write_protect : 1;
	uint8_t copy_flag          : 1;
	uint8_t file_format_group  : 1;

	uint8_t end_bit            : 1;
	uint8_t crc                : 7;
} mmc_csd_t;


static inline uint32_t mmc_tran_speed_to_hz(uint8_t tran_speed)
{
	const uint8_t tv[16] =
	{
		 0, 10, 12, 13,
		15, 20, 25, 30,
		35, 40, 45, 50,
		55, 60, 70, 80,
	};

	uint32_t speed = tv[(tran_speed >> 3) & 0xF];
	for (uint8_t i = 0; i < 4 + (tran_speed & 0x07); i++) speed *= 10;
	return speed;
}

#endif
