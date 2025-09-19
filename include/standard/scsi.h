#ifndef SCSI_H
#define SCSI_H

#include <stdint.h>

typedef enum
{
	SCSI_COMMAND_TEST_UNIT_READY                      = 0x00,
	SCSI_COMMAND_REWIND                               = 0x01,
	SCSI_COMMAND_REQUEST_SENSE                        = 0x03,
	SCSI_COMMAND_FORMAT                               = 0x04,
	SCSI_COMMAND_READ_BLOCK_LIMITS                    = 0x05,
	SCSI_COMMAND_REASSIGN_BLOCKS                      = 0x07,
	SCSI_COMMAND_INITIALIZE_ELEMENT_STATUS            = 0x07,
	SCSI_COMMAND_READ_6                               = 0x08,
	SCSI_COMMAND_WRITE_6                              = 0x0A,
	SCSI_COMMAND_SEEK_6                               = 0x0B,
	SCSI_COMMAND_READ_REVERSE_6                       = 0x0F,
	SCSI_COMMAND_WRITE_FILEMARKS_6                    = 0x10,
	SCSI_COMMAND_SPACE_6                              = 0x11,
	SCSI_COMMAND_INQUIRY                              = 0x12,
	SCSI_COMMAND_VERIFY_6                             = 0x13,
	SCSI_COMMAND_RECOVER_BUFFERED_DATA                = 0x14,
	SCSI_COMMAND_MODE_SELECT_6                        = 0x15,
	SCSI_COMMAND_RESERVE_6                            = 0x16,
	SCSI_COMMAND_RELEASE_6                            = 0x17,
	SCSI_COMMAND_COPY                                 = 0x18,
	SCSI_COMMAND_ERASE_6                              = 0x19,
	SCSI_COMMAND_MODE_SENSE_6                         = 0x1A,
	SCSI_COMMAND_START_STOP_UNIT                      = 0x1B,
	SCSI_COMMAND_LOAD_UNLOAD                          = 0x1B,
	SCSI_COMMAND_RECEIVE_DIAGNOSTIC_RESULTS           = 0x1C,
	SCSI_COMMAND_SEND_DIAGNOSTIC                      = 0x1D,
	SCSI_COMMAND_PREVENT_ALLOW_MEDIUM_REMOVAL         = 0x1E,
	SCSI_COMMAND_READ_FORMAT_CAPACITIES               = 0x23,
	SCSI_COMMAND_READ_CAPACITY_10                     = 0x25,
	SCSI_COMMAND_READ_10                              = 0x28,
	SCSI_COMMAND_READ_GENERATION                      = 0x29,
	SCSI_COMMAND_WRITE_10                             = 0x2A,
	SCSI_COMMAND_SEEK_10                              = 0x2B,
	SCSI_COMMAND_LOCATE_10                            = 0x2B,
	SCSI_COMMAND_ERASE_10                             = 0x2C,
	SCSI_COMMAND_READ_UPDATED_BLOCK                   = 0x2D,
	SCSI_COMMAND_WRITE_AND_VERIFY_10                  = 0x2E,
	SCSI_COMMAND_VERIFY_10                            = 0x2F,
	SCSI_COMMAND_SET_LIMITS_10                        = 0x33,
	SCSI_COMMAND_PRE_FETCH_10                         = 0x34,
	SCSI_COMMAND_READ_POSITION                        = 0x34,
	SCSI_COMMAND_SYNCHRONIZE_CACHE_10                 = 0x35,
	SCSI_COMMAND_LOCK_UNLOCK_CACHE_10                 = 0x36,
	SCSI_COMMAND_READ_DEFECT_DATA_10                  = 0x37,
	SCSI_COMMAND_INITIALIZE_ELEMENT_STATUS_WITH_RANGE = 0x37,
	SCSI_COMMAND_MEDIUM_SCAN                          = 0x38,
	SCSI_COMMAND_COMPARE                              = 0x39,
	SCSI_COMMAND_COPY_AND_VERIFY                      = 0x3A,
	SCSI_COMMAND_WRITE_BUFFER                         = 0x3B,
	SCSI_COMMAND_READ_BUFFER                          = 0x3C,
	SCSI_COMMAND_UPDATE_BLOCK                         = 0x3D,
	SCSI_COMMAND_READ_LONG_10                         = 0x3E,
	SCSI_COMMAND_WRITE_LONG_10                        = 0x3F,
	SCSI_COMMAND_CHANGE_DEFINITION                    = 0x40,
	SCSI_COMMAND_WRITE_SAME_10                        = 0x41,
	SCSI_COMMAND_UNMAP                                = 0x42,
	SCSI_COMMAND_READ_TOC_PMA_ATIP                    = 0x43,
	SCSI_COMMAND_REPORT_DENSITY_SUPPORT               = 0x44,
	SCSI_COMMAND_PLAY_AUDIO_10                        = 0x45,
	SCSI_COMMAND_GET_CONFIGURATION                    = 0x46,
	SCSI_COMMAND_PLAY_AUDIO_MSF                       = 0x47,
	SCSI_COMMAND_SANITIZE                             = 0x48,
	SCSI_COMMAND_GET_EVENT_STATUS_NOTIFICATION        = 0x4A,
	SCSI_COMMAND_PAUSE_RESUME                         = 0x4B,
	SCSI_COMMAND_LOG_SELECT                           = 0x4C,
	SCSI_COMMAND_LOG_SENSE                            = 0x4D,
	SCSI_COMMAND_XDWRITE_10                           = 0x50,
	SCSI_COMMAND_XPWRITE_10                           = 0x51,
	SCSI_COMMAND_READ_DISC_INFORMATION                = 0x51,
	SCSI_COMMAND_XDREAD_10                            = 0x52,
	SCSI_COMMAND_XDWRITEREAD_10                       = 0x53,
	SCSI_COMMAND_SEND_OPC_INFORMATION                 = 0x54,
	SCSI_COMMAND_MODE_SELECT_10                       = 0x55,
	SCSI_COMMAND_RESERVE_10                           = 0x56,
	SCSI_COMMAND_RELEASE_10                           = 0x57,
	SCSI_COMMAND_REPAIR_TRACK                         = 0x58,
	SCSI_COMMAND_MODE_SENSE_10                        = 0x5A,
	SCSI_COMMAND_CLOSE_TRACK_SESSION                  = 0x5B,
	SCSI_COMMAND_READ_BUFFER_CAPACITY                 = 0x5C,
	SCSI_COMMAND_SEND_CUE_SHEET                       = 0x5D,
	SCSI_COMMAND_PERSISTENT_RESERVE_IN                = 0x5E,
	SCSI_COMMAND_PERSISTENT_RESERVE_OUT               = 0x5F,
	SCSI_COMMAND_EXTENDED_CDB                         = 0x7E,
	SCSI_COMMAND_VARIABLE_LENGTH_CDB                  = 0x7F,
	SCSI_COMMAND_XDWRITE_EXTENDED_16                  = 0x80,
	SCSI_COMMAND_WRITE_FILEMARKS_16                   = 0x80,
	SCSI_COMMAND_READ_REVERSE_16                      = 0x81,
	SCSI_COMMAND_THIRD_PARTY_COPY_OUT_COMMANDS        = 0x83,
	SCSI_COMMAND_THIRD_PARTY_COPY_IN_COMMANDS         = 0x84,
	SCSI_COMMAND_ATA_PASS_THROUGH_16                  = 0x85,
	SCSI_COMMAND_ACCESS_CONTROL_IN                    = 0x86,
	SCSI_COMMAND_ACCESS_CONTROL_OUT                   = 0x87,
	SCSI_COMMAND_READ_16                              = 0x88,
	SCSI_COMMAND_COMPARE_AND_WRITE                    = 0x89,
	SCSI_COMMAND_WRITE_16                             = 0x8A,
	SCSI_COMMAND_ORWRITE                              = 0x8B,
	SCSI_COMMAND_READ_ATTRIBUTE                       = 0x8C,
	SCSI_COMMAND_WRITE_ATTRIBUTE                      = 0x8D,
	SCSI_COMMAND_WRITE_AND_VERIFY_16                  = 0x8E,
	SCSI_COMMAND_VERIFY_16                            = 0x8F,
	SCSI_COMMAND_PRE_FETCH_16                         = 0x90,
	SCSI_COMMAND_SYNCHRONIZE_CACHE_16                 = 0x91,
	SCSI_COMMAND_SPACE_16                             = 0x91,
	SCSI_COMMAND_LOCK_UNLOCK_CACHE_16                 = 0x92,
	SCSI_COMMAND_LOCATE_16                            = 0x92,
	SCSI_COMMAND_WRITE_SAME_16                        = 0x93,
	SCSI_COMMAND_ERASE_16                             = 0x93,
	SCSI_COMMAND_SERVICE_ACTION_BIDIRECTIONAL         = 0x9D,
	SCSI_COMMAND_SERVICE_ACTION_IN_16                 = 0x9E,
	SCSI_COMMAND_SERVICE_ACTION_OUT_16                = 0x9F,
	SCSI_COMMAND_REPORT_LUNS                          = 0xA0,
	SCSI_COMMAND_ATA_PASS_THROUGH_12                  = 0xA1,
	SCSI_COMMAND_SECURITY_PROTOCOL_IN                 = 0xA2,
	SCSI_COMMAND_MAINTENANCE_IN                       = 0xA3,
	SCSI_COMMAND_MAINTENANCE_OUT                      = 0xA4,
	SCSI_COMMAND_REPORT_KEY                           = 0xA4,
	SCSI_COMMAND_MOVE_MEDIUM                          = 0xA5,
	SCSI_COMMAND_PLAY_AUDIO_12                        = 0xA5,
	SCSI_COMMAND_EXCHANGE_MEDIUM                      = 0xA6,
	SCSI_COMMAND_MOVE_MEDIUM_ATTACHED                 = 0xA7,
	SCSI_COMMAND_READ_12                              = 0xA8,
	SCSI_COMMAND_SERVICE_ACTION_OUT_12                = 0xA9,
	SCSI_COMMAND_WRITE_12                             = 0xAA,
	SCSI_COMMAND_SERVICE_ACTION_IN_12                 = 0xAB,
	SCSI_COMMAND_ERASE_12                             = 0xAC,
	SCSI_COMMAND_READ_DVD_STRUCTURE                   = 0xAD,
	SCSI_COMMAND_WRITE_AND_VERIFY_12                  = 0xAE,
	SCSI_COMMAND_VERIFY_12                            = 0xAF,
	SCSI_COMMAND_SEARCH_DATA_HIGH_12                  = 0xB0,
	SCSI_COMMAND_SEARCH_DATA_EQUAL_12                 = 0xB1,
	SCSI_COMMAND_SEARCH_DATA_LOW_12                   = 0xB2,
	SCSI_COMMAND_SET_LIMITS_12                        = 0xB3,
	SCSI_COMMAND_READ_ELEMENT_STATUS_ATTACHED         = 0xB4,
	SCSI_COMMAND_SECURITY_PROTOCOL_OUT                = 0xB5,
	SCSI_COMMAND_SEND_VOLUME_TAG                      = 0xB6,
	SCSI_COMMAND_READ_DEFECT_DATA_12                  = 0xB7,
	SCSI_COMMAND_READ_ELEMENT_STATUS                  = 0xB8,
	SCSI_COMMAND_READ_CD_MSF                          = 0xB9,
	SCSI_COMMAND_REDUNDANCY_GROUP_IN                  = 0xBA,
	SCSI_COMMAND_REDUNDANCY_GROUP_OUT                 = 0xBB,
	SCSI_COMMAND_SPARE_IN                             = 0xBC,
	SCSI_COMMAND_SPARE_OUT                            = 0xBD,
	SCSI_COMMAND_VOLUME_SET_IN                        = 0xBE,
	SCSI_COMMAND_VOLUME_SET_OUT                       = 0xBF,
} scsi_command_e;

static inline uint8_t scsi_command_length(scsi_command_e command)
{
	if (command < 0x20)
		return 6;
	else if (command < 0x60)
		return 10;
	else if (command < 0x80)
		return 0;
	else if (command < 0xA0)
		return 16;
	else if (command < 0xC0)
		return 12;
	return 0;
}


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;
	uint8_t res4_1[4];
	uint8_t control;
} scsi_cdb_test_unit_ready_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;
	uint8_t res2_1[2];
	uint8_t allocation_length;
	uint8_t control;
} scsi_cdb_request_sense_t;

typedef enum
{
	SCSI_SENSE_RESPONSE_CODE_CURRENT_FIXED       = 0x70,
	SCSI_SENSE_RESPONSE_CODE_DEFERRED_FIXED      = 0x71,
	SCSI_SENSE_RESPONSE_CODE_CURRENT_DESCRIPTOR  = 0x72,
	SCSI_SENSE_RESPONSE_CODE_DEFERRED_DESCRIPTOR = 0x73,
} scsi_sense_response_code_e;

typedef enum
{
	SCSI_SENSE_KEY_NO_SENSE        = 0x0,
	SCSI_SENSE_KEY_RECOVERED_ERROR = 0x1,
	SCSI_SENSE_KEY_NOT_READY       = 0x2,
	SCSI_SENSE_KEY_MEDIUM_ERROR    = 0x3,
	SCSI_SENSE_KEY_HARDWARE_ERROR  = 0x4,
	SCSI_SENSE_KEY_ILLEGAL_REQUEST = 0x5,
	SCSI_SENSE_KEY_UNIT_ATTENTION  = 0x6,
	SCSI_SENSE_KEY_DATA_PROTECT    = 0x7,
	SCSI_SENSE_KEY_BLANK_CHECK     = 0x8,
	SCSI_SENSE_KEY_FIRMWARE_ERROR  = 0x9,
	SCSI_SENSE_KEY_COPY_ABORTED    = 0xA,
	SCSI_SENSE_KEY_ABORTED_COMMAND = 0xB,
	SCSI_SENSE_KEY_EQUAL           = 0xC,
	SCSI_SENSE_KEY_VOLUME_OVERFLOW = 0xD,
	SCSI_SENSE_KEY_MISCOMPARE      = 0xE,
	SCSI_SENSE_KEY_COMPLETED       = 0xF,
} scsi_sense_key_e;

typedef enum
{
	SCSI_SENSE_QUALIFIER_NO_ADDITIONAL_SENSE_INFORMATION               = 0x0000,
	SCSI_SENSE_QUALIFIER_NO_INDEX_LOGICAL_BLOCK_SIGNAL                 = 0x0001,
	SCSI_SENSE_QUALIFIER_NO_SEEK_COMPLETE                              = 0x0002,
	SCSI_SENSE_QUALIFIER_PERIPHERAL_DEVICE_WRITE_FAULT                 = 0x0003,
	SCSI_SENSE_QUALIFIER_WRITE_ERROR                                   = 0x000C,
	SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR                        = 0x0011,
	SCSI_SENSE_QUALIFIER_INVALID_COMMAND_OPERATION_CODE                = 0x0020,
	SCSI_SENSE_QUALIFIER_MEDIUM_NOT_PRESENT                            = 0x003A,
	SCSI_SENSE_QUALIFIER_INVALID_FIELD_IN_CDB                          = 0x0024,
	SCSI_SENSE_QUALIFIER_PERIPHERAL_LUN_NOT_READY_CAUSE_NOT_REPORTABLE = 0x0004,
	SCSI_SENSE_QUALIFIER_PERIPHERAL_LUN_NOT_READY_BECOMING_READY       = 0x0104,
	SCSI_SENSE_QUALIFIER_PERIPHERAL_LUN_NOT_READY_START_UNIT_REQUIRED  = 0x0204,
	SCSI_SENSE_QUALIFIER_PARAMETER_VALUE_INVALID                       = 0x0226,
	SCSI_SENSE_QUALIFIER_WRITE_FAULT_DATA_CORRUPTION                   = 0x5603,
} scsi_sense_qualifier_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t  response_code;
	uint8_t  segment_number;
	uint8_t  sense_key;
	uint32_t information;
	uint8_t  additional_sense_length;
	uint32_t command_specific_information;
	uint16_t additional_sense_code_qualifier;
	uint8_t  field_replaceable_unit_code;

	struct
	{
		uint8_t bit_pointer : 3;
		uint8_t bpv         : 1;
		uint8_t reserved    : 2;
		uint8_t cd          : 1;
		uint8_t sksv        : 1;
	};

	uint16_t field_pointer;
	uint16_t res19_18;
} scsi_request_sense_data_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t evpd     : 1;
		uint8_t cmddt    : 1;
		uint8_t res15_10 : 6;
	};

	uint8_t  page_code;
	uint16_t allocation_length;
	uint8_t  control;
} scsi_cdb_inquiry_t;

typedef enum
{
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS_BLOCK        = 0x00,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_SEQUENTIAL_ACCESS          = 0x01,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_PRINTER                    = 0x02,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_PROCESSOR                  = 0x03,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_WRITE_ONCE                 = 0x04,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_CD_DVD                     = 0x05,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_SCANNER                    = 0x06,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_OPTICAL_MEMORY             = 0x07,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_MEDIUM_CHANGER             = 0x08,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_COMMUNICATIONS             = 0x09,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_STORAGE_ARRAY_CONTROLLER   = 0x0C,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_ENCLOSURE_SERVICES         = 0x0D,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_SIMPLIFIED_DIRECT_ACCESS   = 0x0E,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_OPTICAL_CARD_READER_WRITER = 0x0F,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_BRIDGE_CONTROLLER_COMMANDS = 0x10,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_OBJECT_BASED_STORAGE       = 0x11,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_AUTOMATION_DRIVE_INTERFACE = 0x12,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_SECURITY_MANAGER           = 0x13,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_HOST_MANAGED_ZONED_BLOCK   = 0x14,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_WELL_KNOWN_LOGICAL_UNIT    = 0x1E,
	SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_UNKNOWN                    = 0x1F,
} scsi_inquiry_peripheral_device_type_e;

typedef enum
{
	SCSI_INQUIRY_RESPONSE_DATA_FORMAT_DEFAULT = 0x2,
} scsi_inquiry_response_data_format_e;

typedef enum
{
	SCSI_INQUIRY_VERSION_NONE = 0x00,
	SCSI_INQUIRY_VERSION_SPC  = 0x03,
	SCSI_INQUIRY_VERSION_SPC2 = 0x04,
	SCSI_INQUIRY_VERSION_SPC3 = 0x05,
	SCSI_INQUIRY_VERSION_SPC4 = 0x06,
	SCSI_INQUIRY_VERSION_SPC5 = 0x07,
	
} scsi_inquiry_version_e;

typedef struct
__attribute__((__packed__))
{
	struct
	{
		uint8_t peripheral_device_type : 5;
		uint8_t peripheral_qualifier   : 3;
	};

	struct
	{
		uint8_t res14_8 : 7;
		uint8_t rmb     : 1;
	};

	uint8_t version;

	struct
	{
		uint8_t response_data_format : 4;
		uint8_t hisup                : 1;
		uint8_t normaca              : 1;
		uint8_t res31_30             : 2;
	};

	uint8_t additional_length;

	struct
	{
		uint8_t protect  : 1;
		uint8_t res42_41 : 2;
		uint8_t tpc      : 1;
		uint8_t tpgs     : 2;
		uint8_t acc      : 1;
		uint8_t sccs     : 1;
	};

	struct
	{
		uint8_t res51_48 : 4;
		uint8_t multip   : 1;
		uint8_t res53    : 1;
		uint8_t encserv  : 1;
		uint8_t res55    : 1;
	};

	struct
	{
		uint8_t res56    : 1;
		uint8_t cmdque   : 1;
		uint8_t res63_58 : 6;
	};

	char vendor_identification[8];
	char product_identification[16];
	char product_revision_level[4];
} scsi_inquiry_data_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t  operation_code;
	uint8_t  res6_1[6];
	uint16_t allocation_length;

	struct
	{
		uint8_t link           : 1;
		uint8_t flag           : 1;
		uint8_t naca           : 1;
		uint8_t res69_67       : 3;
		uint8_t vendor_specific: 2;
	};
} scsi_cdb_read_format_capacities_t;

typedef enum
{
	SCSI_READ_FORMAT_CAPACITIES_DESCRIPTOR_CODE_UNFORMATTED_MEDIA     = 0b01,
	SCSI_READ_FORMAT_CAPACITIES_DESCRIPTOR_CODE_FORMATTED_MEDIA       = 0b10,
	SCSI_READ_FORMAT_CAPACITIES_DESCRIPTOR_CODE_NO_CARTRIDGE_IN_DRIVE = 0b11,
} scsi_read_format_capacities_descriptor_code_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t number_of_blocks;
	uint32_t block_length;
} scsi_read_format_capacities_entry_t;

typedef struct
__attribute__((__packed__))
{
	// The top low 24-bits are reserved technically.
	uint32_t capacity_list_length;

	scsi_read_format_capacities_entry_t entry[];
} scsi_read_format_capacities_data_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t  operation_code;
	uint8_t  res15_8;
	uint32_t logical_block_address;
	uint8_t  res55_48;
	uint8_t  res63_56;
	uint8_t  res71_64; 
	uint8_t  control;
} scsi_cdb_read_capacity_10_t;

typedef struct
__attribute__((__packed__))
{
	uint32_t returned_logical_block_address;
	uint32_t block_length_in_bytes;
} scsi_read_capacity_10_data_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t  res10_8  : 3;
		uint8_t  dbd      : 1;
		uint8_t  res15_12 : 4;
	};

	struct
	{
		uint8_t page_code : 6;
		uint8_t pc        : 2;
	};

	uint8_t subpage_code;
	uint8_t allocation_length;
	uint8_t control;
} scsi_cdb_mode_sense_6_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t mode_data_length;
	uint8_t medium_type;
	
	struct
	{
		uint8_t res19_16 : 4;
		uint8_t dpofua   : 1;
		uint8_t res22_21 : 2;
		uint8_t wp       : 1;
	};

	uint8_t block_descriptor_length;
} scsi_mode_sense_6_header_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t  res10_8  : 3;
		uint8_t  dbd      : 1;
		uint8_t  llbaa    : 1;
		uint8_t  res15_13 : 3;
	};

	struct
	{
		uint8_t page_code : 6;
		uint8_t pc        : 2;
	};

	uint8_t  subpage_code;
	uint8_t  res39_32;
	uint8_t  res47_40;
	uint8_t  res55_48;
	uint16_t allocation_length;
	uint8_t  control;
} scsi_cdb_mode_sense_10_t;

typedef struct
__attribute__((__packed__))
{
	uint16_t mode_data_length;
	uint8_t  medium_type;	

	struct
	{
		uint8_t res27_24 : 4;
		uint8_t dpofua   : 1;
		uint8_t res30_29 : 2;
		uint8_t wp       : 1;
	};

	struct
	{
		uint8_t longlba  : 1;
		uint8_t res39_33 : 7;
	};

	uint8_t  res47_40;
	uint16_t block_descriptor_length;
} scsi_mode_sense_10_header_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;
	uint8_t logical_block_address[3];
	uint8_t transfer_length;
	uint8_t control;
} scsi_cdb_read_6_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t res9_8    : 2;
		uint8_t rarc      : 1;
		uint8_t fua       : 1;
		uint8_t dpo       : 1;
		uint8_t rdprotect : 3;
	};

	uint32_t logical_block_address;
	uint8_t  group_number;
	uint16_t transfer_length;
	uint8_t  control;
} scsi_cdb_read_10_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t dld2      : 1;
		uint8_t res9      : 1;
		uint8_t rarc      : 1;
		uint8_t fua       : 1;
		uint8_t dpo       : 1;
		uint8_t rdprotect : 3;
	};

	uint64_t logical_block_address;

	struct
	{
		uint8_t group_number : 6;
		uint8_t dld1_0       : 2;
	};

	uint32_t transfer_length;
	uint8_t  control;
} scsi_cdb_read_16_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;
	uint8_t logical_block_address[3];
	uint8_t transfer_length;
	uint8_t control;
} scsi_cdb_write_6_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t res10_8   : 3;
		uint8_t fua       : 1;
		uint8_t dpo       : 1;
		uint8_t wrprotect : 3;
	};

	uint32_t logical_block_address;
	uint8_t  group_number;
	uint16_t transfer_length;
	uint8_t  control;
} scsi_cdb_write_10_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t dld2      : 1;
		uint8_t res10_9   : 2;
		uint8_t fua       : 1;
		uint8_t dpo       : 1;
		uint8_t wrprotect : 3;
	};

	uint64_t logical_block_address;

	struct
	{
		uint8_t group_number : 6;
		uint8_t dld1_0       : 2;
	};

	uint32_t transfer_length;
	uint8_t  control;
} scsi_cdb_write_16_t;


typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;

	struct
	{
		uint8_t immed     : 1;
		uint8_t res15_9   : 7;
	};

	uint8_t res23_16;
	uint8_t power_condition_modifier;

	union
	{
		uint8_t start           : 1;
		uint8_t loej            : 1;
		uint8_t noflush         : 1;
		uint8_t res35           : 1;
		uint8_t power_condition : 4;
	};

	uint8_t  control;
} scsi_cdb_start_stop_unit_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t operation_code;
	uint8_t res15_8;
	uint8_t res23_16;
	uint8_t res31_24;
	uint8_t prevent;
	uint8_t control;
} scsi_cdb_prevent_allow_medium_removal_t;

#endif
