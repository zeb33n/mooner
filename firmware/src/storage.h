#ifndef GBFW_PRIVATE_STORAGE_H
#define GBFW_PRIVATE_STORAGE_H

#include <stdbool.h>
#include <stdint.h>


#define GBFW_STORAGE_BLOCK_SIZE_LOG2 9
#define GBFW_STORAGE_BLOCK_SIZE (1U << GBFW_STORAGE_BLOCK_SIZE_LOG2)

typedef enum
{
	GBFW_STORAGE_STATE_DETECTING             =  0,
	GBFW_STORAGE_STATE_NOT_PRESENT           =  1,
	GBFW_STORAGE_STATE_ERROR                 =  2,
	GBFW_STORAGE_STATE_INITIALIZING          =  3,
	GBFW_STORAGE_STATE_IDLE                  =  4,
	GBFW_STORAGE_STATE_READY                 =  5,
	GBFW_STORAGE_STATE_READ_SINGLE           =  8,
	GBFW_STORAGE_STATE_READ_SINGLE_ACTIVE    =  9,
	GBFW_STORAGE_STATE_READ_MULTIPLE         = 10,
	GBFW_STORAGE_STATE_READ_MULTIPLE_ACTIVE  = 11,
	GBFW_STORAGE_STATE_WRITE_SINGLE          = 12,
	GBFW_STORAGE_STATE_WRITE_SINGLE_ACTIVE   = 13,
	GBFW_STORAGE_STATE_WRITE_MULTIPLE        = 14,
	GBFW_STORAGE_STATE_WRITE_MULTIPLE_ACTIVE = 15,
} gbfw_storage_state_e;


void gbfw_storage_init(void);

bool gbfw_storage_lock(void);
void gbfw_storage_release(void);

gbfw_storage_state_e gbfw_storage_state(void);
uint32_t             gbfw_storage_size(void);

bool gbfw_storage_read_multiple_begin(uint32_t addr, uint16_t count);
bool gbfw_storage_read_multiple_block_begin(uint8_t *block);
bool gbfw_storage_read_multiple_block_end(void);
bool gbfw_storage_read_multiple_end(void);

bool gbfw_storage_write_multiple_begin(uint32_t addr, uint16_t count);
bool gbfw_storage_write_multiple_block_begin(const uint8_t *block);
bool gbfw_storage_write_multiple_block_end(void);
bool gbfw_storage_write_multiple_end(void);


static inline bool gbfw_storage_read_block(uint32_t addr, uint8_t *block)
{
	if (!gbfw_storage_read_multiple_begin(addr, 1))
		return false;
	bool success = gbfw_storage_read_multiple_block_begin(block)
		&& gbfw_storage_read_multiple_block_end();
	gbfw_storage_read_multiple_end();
	return success;
}

static inline bool gbfw_storage_write_block(uint32_t addr, const uint8_t *block)
{
	if (!gbfw_storage_write_multiple_begin(addr, 1))
		return false;
	bool success = gbfw_storage_write_multiple_block_begin(block)
		&& gbfw_storage_write_multiple_block_end();
	gbfw_storage_write_multiple_end();
	return success;
}

#endif
