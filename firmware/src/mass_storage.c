#include <gbfw/usb/mass_storage.h>
#include <gbfw/usb.h>
#include "storage.h"
#include <standard/usb.h>
#include <standard/usb/mass_storage.h>
#include <standard/scsi.h>
#include <hardware/stm32/usb.h>
#include <util/endian.h>
#include <stddef.h>

#define GBFW_SCSI_VENDOR  "Flatmush"
#define GBFW_SCSI_PRODUCT "GameBlaster"

#define GBFW_USB_MASS_STORAGE_STRING_MANUFACTURER u"Flatmush"
#define GBFW_USB_MASS_STORAGE_STRING_PRODUCT      u"GameBlaster"

#define GBFW_USB_MASS_STORAGE__ENDPOINT_IN  1
#define GBFW_USB_MASS_STORAGE__ENDPOINT_OUT (GBFW_USB_MASS_STORAGE__ENDPOINT_IN + 1)
#define GBFW_USB_MASS_STORAGE__ENDPOINT_MAX_PACKET_SIZE 64

// If enabled READ(6) and write(6) are supported however these are rarely used
// and the code paths add 152 bytes to the firmware.
#undef SCSI_READ_WRITE_6_SUPPORT


static const struct
__attribute__((__packed__))
{
	usb_descriptor_device_t        device;
	usb_descriptor_configuration_t configuration;
	usb_descriptor_interface_t     interface;
	usb_descriptor_endpoint_t      endpoint[2];

	struct
	__attribute__((__packed__))
	{
		uint8_t  length;
		uint8_t  descriptor_type;
		uint16_t string[(sizeof(GBFW_USB_MASS_STORAGE_STRING_MANUFACTURER) / 2) - 1];
	} string_manufacturer;

	struct
	__attribute__((__packed__))
	{
		uint8_t  length;
		uint8_t  descriptor_type;
		uint16_t string[(sizeof(GBFW_USB_MASS_STORAGE_STRING_PRODUCT) / 2) - 1];
	} string_product;
} usb_mass_storage__descriptors =
{
	.device =
	{
		.length          = sizeof(usb_descriptor_device_t),
		.descriptor_type = USB_DESCRIPTOR_DEVICE,

		.usb_spec = USB_SPEC_2_0,

		.device_class     = USB_CLASS_PER_INTERFACE,
		.device_sub_class = 0,
		.device_protcol   = 0,

		.max_packet_size = 64,

		// https://github.com/obdev/v-usb/blob/master/usbdrv/USB-IDs-for-free.txt
		.vendor_id  = 0x16C0,
		.product_id = 0x27DF,

		.device        = 0x0100,
		.manufacturer  =      2,
		.product       =      3,
		.serial_number =      1,

		.num_configurations = 1,
	},

	.configuration =
	{
		.length          = sizeof(usb_descriptor_configuration_t),
		.descriptor_type = USB_DESCRIPTOR_CONFIGURATION,

		.total_length = sizeof(usb_descriptor_configuration_t)
			+ sizeof(usb_descriptor_interface_t)
			+ (sizeof(usb_descriptor_endpoint_t) * 2),

		.num_interfaces      = 1,
		.configuration_value = 1,
		.configuration       = 0,

		.attributes.remote_wakeup    = 0,
		.attributes.self_powered     = 0,
		.attributes.usb1_bus_powered = 1,

		.max_power = USB_POWER_MA(500),
	},

	.interface =
	{
		.length          = sizeof(usb_descriptor_interface_t),
		.descriptor_type = USB_DESCRIPTOR_INTERFACE,

		.interface_number  = 0,
		.alternate_setting = 0,
		.num_endpoints     = 2,

		.interface_class     = USB_CLASS_MASS_STORAGE,
		.interface_sub_class = USB_SUB_CLASS_MASS_STORAGE_SCSI_TRANSPARENT,
		.interface_protocol  = USB_PROTOCOL_MASS_STORAGE_BBB,
		.interface           = 0,
	},

	.endpoint[GBFW_USB_MASS_STORAGE__ENDPOINT_IN - 1] =
	{
		.length          = sizeof(usb_descriptor_endpoint_t),
		.descriptor_type = USB_DESCRIPTOR_ENDPOINT,

		.endpoint_address = 0x80 | GBFW_USB_MASS_STORAGE__ENDPOINT_IN,

		.attributes.transfer_type = USB_TRANSFER_TYPE_BULK,
		.attributes.sync_type     = USB_SYNC_TYPE_NONE,
		.attributes.usage_type    = USB_USAGE_TYPE_DATA,

		.max_packet_size = GBFW_USB_MASS_STORAGE__ENDPOINT_MAX_PACKET_SIZE,
		.interval        =  0,
	},

	.endpoint[GBFW_USB_MASS_STORAGE__ENDPOINT_OUT - 1] =
	{
		.length          = sizeof(usb_descriptor_endpoint_t),
		.descriptor_type = USB_DESCRIPTOR_ENDPOINT,

		.endpoint_address = GBFW_USB_MASS_STORAGE__ENDPOINT_OUT,

		.attributes.transfer_type = USB_TRANSFER_TYPE_BULK,
		.attributes.sync_type     = USB_SYNC_TYPE_NONE,
		.attributes.usage_type    = USB_USAGE_TYPE_DATA,

		.max_packet_size = GBFW_USB_MASS_STORAGE__ENDPOINT_MAX_PACKET_SIZE,
		.interval        =  0,
	},

	.string_manufacturer =
	{
		.length          = sizeof(GBFW_USB_MASS_STORAGE_STRING_MANUFACTURER),
		.descriptor_type = USB_DESCRIPTOR_STRING,
		.string          = GBFW_USB_MASS_STORAGE_STRING_MANUFACTURER,
	},

	.string_product =
	{
		.length          = sizeof(GBFW_USB_MASS_STORAGE_STRING_PRODUCT),
		.descriptor_type = USB_DESCRIPTOR_STRING,
		.string          = GBFW_USB_MASS_STORAGE_STRING_PRODUCT,
	},
};

static const scsi_request_sense_data_t gbfw_usb_mass_storage__scsi_sense_init =
{
	.response_code                   = SCSI_SENSE_RESPONSE_CODE_CURRENT_FIXED,
	.segment_number                  = 0x00,
	.sense_key                       = SCSI_SENSE_KEY_NO_SENSE,
	.information                     = 0x00000000,
	.additional_sense_length         = 12,
	.command_specific_information    = 0x00000000,
	.additional_sense_code_qualifier = SCSI_SENSE_QUALIFIER_NO_ADDITIONAL_SENSE_INFORMATION,
	.field_replaceable_unit_code     = 0x00,
	.bit_pointer                     = 0b000,
	.bpv                             = false,
	.reserved                        = 0b00,
	.cd                              = 0,
	.sksv                            = false,
	.field_pointer                   = 0x0000,
 	.res19_18                        = 0x0000,
};

static const scsi_inquiry_data_t gbfw_usb_mass_storage__scsi_inquiry =
{
	.peripheral_device_type = SCSI_INQUIRY_PERIPHERAL_DEVICE_TYPE_DIRECT_ACCESS_BLOCK,
	.peripheral_qualifier	= 0b000,
	.res14_8                = 0b0000000,
	.rmb                    = true,
	.version                = SCSI_INQUIRY_VERSION_SPC2,
	.response_data_format   = SCSI_INQUIRY_RESPONSE_DATA_FORMAT_DEFAULT,
	.hisup                  = false,
	.normaca                = false,
	.res31_30               = 0b00,
	.additional_length      = 31,
	.protect                = false,
	.res42_41               = 0b00,
	.tpc                    = false,
	.tpgs                   = 0b00,
	.acc                    = false,
	.sccs                   = false,
	.res51_48               = 0b0000,
	.multip                 = false,
	.res53                  = 0,
	.encserv                = false,
	.res55                  = 0,
	.res56                  = 0,
	.cmdque                 = false,
	.res63_58               = 0b000000,
	.vendor_identification  = GBFW_SCSI_VENDOR,
	.product_identification = GBFW_SCSI_PRODUCT,
	.product_revision_level = "1.00",
};



static inline uint16_t gbfw_usb_mass_storage__block_ptr(uint8_t i)
{
	return (i * (GBFW_STORAGE_BLOCK_SIZE  / 2));
}

static inline volatile void *gbfw_usb_mass_storage__block(uint8_t i)
{
	return STM32_USB_SRAM_PTR(gbfw_usb_mass_storage__block_ptr(i));
}

static inline uint16_t gbfw_usb_mass_storage__cbw_ptr(void)
{
	return GBFW_STORAGE_BLOCK_SIZE + (GBFW_STORAGE_BLOCK_SIZE  / 2);
}

static inline volatile usb_mass_storage_cbw_t *gbfw_usb_mass_storage__cbw(void)
{
	return STM32_USB_SRAM_PTR(gbfw_usb_mass_storage__cbw_ptr());
}

static inline uint16_t gbfw_usb_mass_storage__block_address_ptr(void)
{
	return gbfw_usb_mass_storage__cbw_ptr()
		+ offsetof(usb_mass_storage_cbw_t, command_block)
		+ offsetof(scsi_cdb_read_16_t, transfer_length);
}

static inline volatile uint32_t *gbfw_usb_mass_storage__block_address(void)
{
	return STM32_USB_SRAM_PTR(gbfw_usb_mass_storage__block_address_ptr());
}

static inline uint16_t gbfw_usb_mass_storage__csw_ptr(void)
{
	return gbfw_usb_mass_storage__cbw_ptr()
		+ STM32_USB_SRAM_SIZE_ROUND16(sizeof(usb_mass_storage_cbw_t));
}

static inline volatile usb_mass_storage_csw_t *gbfw_usb_mass_storage__csw(void)
{
	return STM32_USB_SRAM_PTR(gbfw_usb_mass_storage__csw_ptr());
}

static inline uint16_t gbfw_usb_mass_storage__sense_data_ptr(void)
{
	return gbfw_usb_mass_storage__csw_ptr()
		+ STM32_USB_SRAM_SIZE_ROUND16(sizeof(usb_mass_storage_csw_t));
}

static inline volatile scsi_request_sense_data_t *gbfw_usb_mass_storage__sense_data(void)
{
	return STM32_USB_SRAM_PTR(gbfw_usb_mass_storage__sense_data_ptr());
}



static void gbfw_usb_mass_storage__buffer_init(void)
{
	// Block cache doesn't need initializing.

	// CBW is a receive buffer so doesn't need initializing.

	// Initialize constant fields in CSW.
	volatile usb_mass_storage_csw_t *csw = gbfw_usb_mass_storage__csw();
	stm32_usb_sram_write32(&csw->signature, USB_MASS_STORAGE_CSW_SIGNATURE);

	uint16_t sense_data = gbfw_usb_mass_storage__sense_data_ptr();
	stm32_usb_sram_copy_in(sense_data,
		&gbfw_usb_mass_storage__scsi_sense_init,
		sizeof(gbfw_usb_mass_storage__scsi_sense_init));
}

typedef union
{
	struct
	{
		uint8_t stopped : 1;
		uint8_t ejected : 1;
		uint8_t rbuff   : 1;
		uint8_t wbuff   : 1;
		uint8_t res7_4  : 4;
	};

	uint8_t mask;
} gbfw_usb_mass_storage__flags_t;

static volatile gbfw_usb_mass_storage__flags_t gbfw_usb_mass_storage__flags = { .mask = 0x00 };

static void gbfw_usb_mass_storage__disconnect(void)
{
	gbfw_storage_read_multiple_end();
	gbfw_storage_write_multiple_end();
}

static void gbfw_usb_mass_storage__soft_reset(void)
{
	gbfw_usb_endpoint_set_packet(
		GBFW_USB_MASS_STORAGE__ENDPOINT_IN, 0, 0);

	gbfw_usb_endpoint_set_packet(
		GBFW_USB_MASS_STORAGE__ENDPOINT_OUT,
		gbfw_usb_mass_storage__cbw_ptr(),
		sizeof(usb_mass_storage_cbw_t));

	gbfw_usb_mass_storage__disconnect();
}

static void gbfw_usb_mass_storage__reset(void)
{
	gbfw_usb_mass_storage__flags.mask = 0x00;

	gbfw_usb_mass_storage__soft_reset();
}

static bool gbfw_usb_mass_storage__setup(
	volatile const usb_request_t *request,
	volatile uint16_t *addr, uint16_t *len)
{
	// The only other valid request would be Get Max LUN which we ignore.
	if (request->request != USB_REQUEST_MASS_STORAGE_BOMSR)
		return false;

	// Ignore packet pointers as we don't need a data stage.
	(void)addr;
	(void)len;

	// Reset mass storage state only.
	gbfw_usb_mass_storage__soft_reset();
	return true;
}

static bool gbfw_usb_mass_storage__endpoint_stall(uint8_t ep, bool stall)
{
	// There's no value in the host being able to stall an endpoint.
	if (stall) return false;

	if (ep == GBFW_USB_MASS_STORAGE__ENDPOINT_IN)
	{
		gbfw_usb_endpoint_set_packet(
			GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
			gbfw_usb_mass_storage__csw_ptr(),
			sizeof(usb_mass_storage_csw_t));
		return true;
	}
	else if (ep == GBFW_USB_MASS_STORAGE__ENDPOINT_OUT)
	{
		gbfw_usb_endpoint_set_packet(
			GBFW_USB_MASS_STORAGE__ENDPOINT_OUT,
			gbfw_usb_mass_storage__cbw_ptr(),
			sizeof(usb_mass_storage_cbw_t));
		return true;
	}

	return false;
}


static void gbfw_usb_mass_storage__scsi_error(
	scsi_sense_key_e key, scsi_sense_qualifier_e qualifier)
{
	volatile usb_mass_storage_cbw_t *cbw = gbfw_usb_mass_storage__cbw();
	volatile usb_mass_storage_csw_t *csw = gbfw_usb_mass_storage__csw();

	bool success = (key == SCSI_SENSE_KEY_NO_SENSE);

	stm32_usb_sram_write8(&csw->status, (success
		? USB_MASS_STORAGE_CSW_STATUS_COMMAND_PASSED
		: USB_MASS_STORAGE_CSW_STATUS_COMMAND_FAILED));

	uint32_t transfer_length = stm32_usb_sram_read32(&cbw->data_transfer_length);
	uint32_t residue = stm32_usb_sram_read32(&csw->data_residue) + transfer_length;

	stm32_usb_sram_write32(&csw->data_residue, residue);
	stm32_usb_sram_write32(&cbw->data_transfer_length, 0);

	uint8_t flags = stm32_usb_sram_read8(&cbw->flags.mask);
	bool direction = flags >> 7;

	if ((residue > 0) && !direction)
	{
		// Stall so that the host knows reception ended early.
		gbfw_usb_endpoint_stall(GBFW_USB_MASS_STORAGE__ENDPOINT_OUT);
	}

	if ((residue > 0) && direction)
	{
		// Stall so that the host knows transmission ended early.
		gbfw_usb_endpoint_stall(GBFW_USB_MASS_STORAGE__ENDPOINT_IN);
	}
	else
	{
		gbfw_usb_endpoint_set_packet(
			GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
			gbfw_usb_mass_storage__csw_ptr(),
			sizeof(usb_mass_storage_csw_t));
	}

	volatile scsi_request_sense_data_t *sense
		= gbfw_usb_mass_storage__sense_data();
	stm32_usb_sram_write8(&sense->sense_key, key);
	stm32_usb_sram_write16(&sense->additional_sense_code_qualifier, qualifier);
}

static inline void gbfw_usb_mass_storage__scsi_success(void)
{
	gbfw_usb_mass_storage__scsi_error(
		SCSI_SENSE_KEY_NO_SENSE,
		SCSI_SENSE_QUALIFIER_NO_ADDITIONAL_SENSE_INFORMATION);
}

static void gbfw_usb_mass_storage__queue_data(
	uint8_t ep, uint16_t buff, uint32_t size)
{
	volatile usb_mass_storage_cbw_t *cbw = gbfw_usb_mass_storage__cbw();
	volatile usb_mass_storage_csw_t *csw = gbfw_usb_mass_storage__csw();

	uint32_t transfer_length = stm32_usb_sram_read32(&cbw->data_transfer_length);
	if (size >= transfer_length)
	{
		size = transfer_length;
	}
	else
	{
		stm32_usb_sram_write32(&csw->data_residue, (transfer_length - size));
		stm32_usb_sram_write32(&cbw->data_transfer_length, size);
	}

	if (size == 0)
	{
		// No data has been requested so respond with success.
		gbfw_usb_mass_storage__scsi_success();
		return;
	}

	if (size > GBFW_USB_MASS_STORAGE__ENDPOINT_MAX_PACKET_SIZE)
		size = GBFW_USB_MASS_STORAGE__ENDPOINT_MAX_PACKET_SIZE;

	gbfw_usb_endpoint_set_packet(ep, buff, size);
}


static void gbfw_usb_mass_storage__scsi_read_write(bool write, uint32_t address)
{
	volatile usb_mass_storage_cbw_t *cbw = gbfw_usb_mass_storage__cbw();
	uint32_t transfer_length = stm32_usb_sram_read32(&cbw->data_transfer_length);

	// When writing we can only write full blocks.
	if (write && ((transfer_length % GBFW_STORAGE_BLOCK_SIZE) != 0))
	{
		// We don't know if error is in CDB but this is easier than
		// trying to handle at the USB level where errors are undefined.
		gbfw_usb_mass_storage__scsi_error(
			SCSI_SENSE_KEY_ILLEGAL_REQUEST,
			SCSI_SENSE_QUALIFIER_INVALID_FIELD_IN_CDB);
		return;
	}

	uint32_t blocks = (transfer_length + (GBFW_STORAGE_BLOCK_SIZE - 1))
		>> GBFW_STORAGE_BLOCK_SIZE_LOG2;

	// MMC cards can't read more than 65535 consecutive blocks.
	if ((blocks >> 16) != 0)
	{
		gbfw_usb_mass_storage__scsi_error(
			SCSI_SENSE_KEY_ILLEGAL_REQUEST,
			SCSI_SENSE_QUALIFIER_INVALID_FIELD_IN_CDB);
		return;
	}

	uint32_t storage_size = gbfw_storage_size();
	if ((address + blocks) > storage_size)
	{
		gbfw_usb_mass_storage__scsi_error(
			SCSI_SENSE_KEY_ILLEGAL_REQUEST,
			SCSI_SENSE_QUALIFIER_INVALID_FIELD_IN_CDB);
		return;
	}

	if (write)
	{
		if (!gbfw_storage_write_multiple_begin(address, blocks))
		{
			gbfw_usb_mass_storage__scsi_error(
				SCSI_SENSE_KEY_MEDIUM_ERROR,
				SCSI_SENSE_QUALIFIER_WRITE_ERROR);
			return;
		}
	}
	else
	{
		if (!gbfw_storage_read_multiple_begin(address, blocks)
			|| !gbfw_storage_read_multiple_block_begin((void *)gbfw_usb_mass_storage__block(0))
			|| !gbfw_storage_read_multiple_block_end())
		{
			gbfw_storage_read_multiple_end();
			gbfw_usb_mass_storage__scsi_error(
				SCSI_SENSE_KEY_MEDIUM_ERROR,
				SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR);
			return;
		}
	}

	// Store address in a known place to use as state.
	stm32_usb_sram_write32(gbfw_usb_mass_storage__block_address(), address);

	gbfw_usb_mass_storage__flags.rbuff = 0;
	gbfw_usb_mass_storage__flags.wbuff = 0;

	if (write)
	{
		gbfw_usb_mass_storage__queue_data(
			GBFW_USB_MASS_STORAGE__ENDPOINT_OUT,
			gbfw_usb_mass_storage__block_ptr(0),
			stm32_usb_sram_read32(&cbw->data_transfer_length));
	}
	else
	{
		gbfw_usb_mass_storage__queue_data(
			GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
			gbfw_usb_mass_storage__block_ptr(0),
			stm32_usb_sram_read32(&cbw->data_transfer_length));
	}
}

static inline bool gbfw_usb_mass_storage__scsi_command_is_medium_access(uint8_t command)
{
	switch (command)
	{
		case SCSI_COMMAND_TEST_UNIT_READY:
		case SCSI_COMMAND_READ_CAPACITY_10:

		// Technically this isn't a medium access, but this isn't an
		// optical drive either so we treat it as a synonym of
		// READ CAPACITY.
		case SCSI_COMMAND_READ_FORMAT_CAPACITIES:

#ifdef SCSI_READ_WRITE_6_SUPPORT
		case SCSI_COMMAND_READ_6:
		case SCSI_COMMAND_WRITE_6:
			return true;
#endif

		case SCSI_COMMAND_READ_10:
		case SCSI_COMMAND_WRITE_10:
			return true;

		default:
			break;
	}

	return false;
}

static bool gbfw_usb_mass_storage__scsi_process(volatile void *cmd, uint8_t len)
{
	scsi_command_e command = stm32_usb_sram_read8(cmd);

	if (len != scsi_command_length(command))
		return false;

	if (gbfw_usb_mass_storage__flags.stopped
		&& gbfw_usb_mass_storage__scsi_command_is_medium_access(command))
		
	{
		gbfw_usb_mass_storage__scsi_error(
			SCSI_SENSE_KEY_NOT_READY,
			(gbfw_usb_mass_storage__flags.ejected
				? SCSI_SENSE_QUALIFIER_MEDIUM_NOT_PRESENT
				: SCSI_SENSE_QUALIFIER_PERIPHERAL_LUN_NOT_READY_START_UNIT_REQUIRED));
		return true;
	}

	switch (command)
	{
		case SCSI_COMMAND_PREVENT_ALLOW_MEDIUM_REMOVAL:
		{
			volatile scsi_cdb_prevent_allow_medium_removal_t *pamr = cmd;
			uint8_t prevent = stm32_usb_sram_read8(&pamr->prevent) & 0x03;
			if (prevent != 0)
			{
				// We can't prevent a user from removing the device.
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_ILLEGAL_REQUEST,
					SCSI_SENSE_QUALIFIER_INVALID_FIELD_IN_CDB);
			}
			else
			{
				// We must support this to allow the device to be ejected.
				gbfw_usb_mass_storage__scsi_success();
			}
		} break;

		case SCSI_COMMAND_START_STOP_UNIT:
		{
			volatile scsi_cdb_start_stop_unit_t *start_stop_unit = cmd;
			uint8_t flags = stm32_usb_sram_read8(
				(volatile void *)((uintptr_t)&start_stop_unit->control - 1));
			bool start      = flags & 0x01;
			bool load_eject = (flags & 0x02) >> 1;

			gbfw_usb_mass_storage__flags.stopped = !start;
			if (load_eject)
				gbfw_usb_mass_storage__flags.ejected = !start;
			gbfw_usb_mass_storage__scsi_success();
		} break;

		case SCSI_COMMAND_TEST_UNIT_READY:
		{
			gbfw_usb_mass_storage__scsi_success();
		} break;

		case SCSI_COMMAND_REQUEST_SENSE:
		{
			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				gbfw_usb_mass_storage__sense_data_ptr(),
				sizeof(scsi_request_sense_data_t));
		} break;

		case SCSI_COMMAND_INQUIRY:
		{
			uint16_t inquiry_data = gbfw_usb_mass_storage__block_ptr(0);
			stm32_usb_sram_copy_in(inquiry_data,
				&gbfw_usb_mass_storage__scsi_inquiry,
				sizeof(gbfw_usb_mass_storage__scsi_inquiry));
			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				inquiry_data, sizeof(scsi_inquiry_data_t));
		} break;

		case SCSI_COMMAND_READ_CAPACITY_10:
		{
			volatile scsi_read_capacity_10_data_t *data
				= gbfw_usb_mass_storage__block(0);
			stm32_usb_sram_write32(
				&data->returned_logical_block_address,
				ENDIAN_SWAP32(gbfw_storage_size() - 1));
			stm32_usb_sram_write32(
				&data->block_length_in_bytes,
				ENDIAN_SWAP32(GBFW_STORAGE_BLOCK_SIZE));

			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				gbfw_usb_mass_storage__block_ptr(0),
				sizeof(scsi_read_capacity_10_data_t));
		} break;

		case SCSI_COMMAND_READ_FORMAT_CAPACITIES:
		{
			volatile scsi_read_format_capacities_data_t *data
				= gbfw_usb_mass_storage__block(0);

			static const uint8_t hsize = sizeof(scsi_read_format_capacities_data_t);
			static const uint8_t entries = 2;
			static const uint8_t esize = sizeof(scsi_read_format_capacities_entry_t);

			stm32_usb_sram_write32(
				&data->capacity_list_length,
				ENDIAN_SWAP32(esize * entries));

			uint32_t blocks = gbfw_storage_size();

			uint32_t ud = SCSI_READ_FORMAT_CAPACITIES_DESCRIPTOR_CODE_UNFORMATTED_MEDIA << 24;
			stm32_usb_sram_write32(
				&data->entry[0].number_of_blocks,
				ENDIAN_SWAP32(blocks));
			stm32_usb_sram_write32(
				&data->entry[0].block_length,
				ENDIAN_SWAP32(GBFW_STORAGE_BLOCK_SIZE | ud));

			uint32_t fd = SCSI_READ_FORMAT_CAPACITIES_DESCRIPTOR_CODE_FORMATTED_MEDIA << 24;
			stm32_usb_sram_write32(
				&data->entry[1].number_of_blocks,
				ENDIAN_SWAP32(blocks));
			stm32_usb_sram_write32(
				&data->entry[1].block_length,
				ENDIAN_SWAP32(GBFW_STORAGE_BLOCK_SIZE | fd));

			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				gbfw_usb_mass_storage__block_ptr(0),
				(hsize + (esize * entries)));
		} break;

		case SCSI_COMMAND_MODE_SENSE_6:
		{
			volatile scsi_mode_sense_6_header_t *header
				= gbfw_usb_mass_storage__block(0);
			// Quickest way to write 3, 0, 0, 0 as no unalignment.
			stm32_usb_sram_write32(&header->mode_data_length, 3);

			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				gbfw_usb_mass_storage__block_ptr(0),
				sizeof(scsi_mode_sense_6_header_t));
		} break;

		case SCSI_COMMAND_MODE_SENSE_10:
		{
			volatile scsi_mode_sense_10_header_t *header
				= gbfw_usb_mass_storage__block(0);
			stm32_usb_sram_write16(&header->mode_data_length, ENDIAN_SWAP16(6));
			stm32_usb_sram_write32(&header->medium_type, 0);
			stm32_usb_sram_write16(&header->block_descriptor_length, 0);

			gbfw_usb_mass_storage__queue_data(
				GBFW_USB_MASS_STORAGE__ENDPOINT_IN,
				gbfw_usb_mass_storage__block_ptr(0),
				sizeof(scsi_mode_sense_10_header_t));
		} break;

#ifdef SCSI_READ_WRITE_6_SUPPORT
		case SCSI_COMMAND_READ_6:
		case SCSI_COMMAND_WRITE_6:
		{
			volatile scsi_cdb_read_6_t *read6 = cmd;
			uint32_t address = ((stm32_usb_sram_read8(&read6->logical_block_address[0]) & 0x1F) << 16)
				+ (stm32_usb_sram_read8(&read6->logical_block_address[1]) << 8)
				+ stm32_usb_sram_read8(&read6->logical_block_address[2]);
			gbfw_usb_mass_storage__scsi_read_write((command == SCSI_COMMAND_WRITE_6), address);
		} break;
#endif

		case SCSI_COMMAND_READ_10:
		case SCSI_COMMAND_WRITE_10:
		{
			volatile scsi_cdb_read_10_t *read10 = cmd;
			gbfw_usb_mass_storage__scsi_read_write((command == SCSI_COMMAND_WRITE_10),
				ENDIAN_SWAP32(stm32_usb_sram_read32_unaligned(&read10->logical_block_address)));
		} break;

		default:
		{
			gbfw_usb_mass_storage__scsi_error(
				SCSI_SENSE_KEY_ILLEGAL_REQUEST,
				SCSI_SENSE_QUALIFIER_INVALID_COMMAND_OPERATION_CODE);
		} break;
	}

	return true;
}

static bool gbfw_usb_mass_storage__cbw_process(volatile usb_mass_storage_cbw_t *cbw)
{
	uint32_t signature = stm32_usb_sram_read32(&cbw->signature);

	if ((signature != USB_MASS_STORAGE_CBW_SIGNATURE)
		|| (stm32_usb_sram_read8(&cbw->lun) != 0))
		return false;

	volatile usb_mass_storage_csw_t *csw = gbfw_usb_mass_storage__csw();
	stm32_usb_sram_write32(&csw->tag, stm32_usb_sram_read32(&cbw->tag));
	stm32_usb_sram_write32(&csw->data_residue, 0);

	return gbfw_usb_mass_storage__scsi_process(
		cbw->command_block, stm32_usb_sram_read8(&cbw->length));
}

static void gbfw_usb_mass_storage__recv(uint8_t ep, uint16_t buff, uint16_t size)
{
	volatile usb_mass_storage_cbw_t *cbw = gbfw_usb_mass_storage__cbw();
	if (buff == gbfw_usb_mass_storage__cbw_ptr())
	{
		if ((size != sizeof(usb_mass_storage_cbw_t))
			|| !gbfw_usb_mass_storage__cbw_process(cbw))
		{
			gbfw_usb_endpoint_stall(ep);
			return;
		}
	}
	else
	{
		uint32_t transfer_length = stm32_usb_sram_read32(
			&cbw->data_transfer_length);

		buff += size;
		if (size >= transfer_length)
			transfer_length = 0;
		else
			transfer_length -= size;
		stm32_usb_sram_write32(&cbw->data_transfer_length, transfer_length);

		if (size > transfer_length)
			size = transfer_length;

		uint16_t block0 = gbfw_usb_mass_storage__block_ptr(0);
		uint16_t block1 = gbfw_usb_mass_storage__block_ptr(1);
		uint16_t block2 = gbfw_usb_mass_storage__block_ptr(2);

		if ((gbfw_usb_mass_storage__flags.rbuff == 0)
			&& (gbfw_usb_mass_storage__flags.wbuff == 0)
			&& (buff == (block0 + GBFW_STORAGE_BLOCK_SIZE)))
		{
			if (!gbfw_storage_write_multiple_block_begin(
				(const void *)gbfw_usb_mass_storage__block(0)))
			{
				gbfw_storage_write_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_WRITE_ERROR);
				return;
			}

			gbfw_usb_mass_storage__flags.rbuff = 1;
		}
		else if ((gbfw_usb_mass_storage__flags.rbuff == 1)
			&& (gbfw_usb_mass_storage__flags.wbuff == 0)
			&& (buff == (block1 + GBFW_STORAGE_BLOCK_SIZE)))
		{
			if (!gbfw_storage_write_multiple_block_end())
			{
				gbfw_storage_write_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_WRITE_ERROR);
				return;
			}

			// This is necessary due to having 1.5 buffers.
			stm32_usb_sram_copy_in(block1,
				(const void *)gbfw_usb_mass_storage__block(2),
				(GBFW_STORAGE_BLOCK_SIZE / 2));
			buff = block2;

			gbfw_usb_mass_storage__flags.wbuff = 1;
		}
		else if ((gbfw_usb_mass_storage__flags.rbuff == 1)
			&& (gbfw_usb_mass_storage__flags.wbuff == 1)
			&& (buff == (block1 + GBFW_STORAGE_BLOCK_SIZE)))
		{
			if (!gbfw_storage_write_multiple_block_begin(
				(const void *)gbfw_usb_mass_storage__block(1)))
			{
				gbfw_storage_write_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_WRITE_ERROR);
				return;
			}

			buff = block0;

			gbfw_usb_mass_storage__flags.rbuff = 0;
		}
		else if ((gbfw_usb_mass_storage__flags.rbuff == 0)
			&& (gbfw_usb_mass_storage__flags.wbuff == 1)
			&& (buff == block1))
		{
			if (!gbfw_storage_write_multiple_block_end())
			{
				gbfw_storage_write_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_WRITE_ERROR);
				return;
			}

			gbfw_usb_mass_storage__flags.wbuff = 0;
		}

		if (size == 0)
		{
			if (!gbfw_storage_write_multiple_end())
			{
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_WRITE_ERROR);
				return;
			}
			else
			{
				gbfw_usb_mass_storage__scsi_success();
			}
		}
		else
		{
			gbfw_usb_endpoint_set_packet(ep, buff, size);
		}
	}
}

static void gbfw_usb_mass_storage__sent(uint8_t ep, uint16_t buff, uint16_t size)
{
	volatile usb_mass_storage_cbw_t *cbw = gbfw_usb_mass_storage__cbw();
	uint32_t transfer_length = stm32_usb_sram_read32(&cbw->data_transfer_length);

	if (transfer_length == 0)
	{
		// CSW confirmation
		gbfw_usb_endpoint_set_packet(
			GBFW_USB_MASS_STORAGE__ENDPOINT_OUT,
			gbfw_usb_mass_storage__cbw_ptr(),
			sizeof(usb_mass_storage_cbw_t));
	}
	else if (size >= transfer_length)
	{
		// Transfer complete
		gbfw_storage_read_multiple_end();
		stm32_usb_sram_write32(&cbw->data_transfer_length, 0);
		gbfw_usb_mass_storage__scsi_success();
	}
	else
	{
		buff += size;
		transfer_length -= size;
		stm32_usb_sram_write32(&cbw->data_transfer_length, transfer_length);

		if (size > transfer_length)
			size = transfer_length;

		uint16_t block0 = gbfw_usb_mass_storage__block_ptr(0);
		uint16_t block1 = gbfw_usb_mass_storage__block_ptr(1);

		if ((gbfw_usb_mass_storage__flags.rbuff == 0)
			&& (gbfw_usb_mass_storage__flags.wbuff == 0)
			&& (buff == (block0 + (GBFW_STORAGE_BLOCK_SIZE / 2)))
			&& (transfer_length > (GBFW_STORAGE_BLOCK_SIZE / 2)))
		{
			// This is necessary due to having 1.5 buffers.
			stm32_usb_sram_copy_in(block0,
				(void *)gbfw_usb_mass_storage__block(1),
				(GBFW_STORAGE_BLOCK_SIZE / 2));
			buff = block0;

			gbfw_usb_mass_storage__flags.wbuff = 1;

			if (!gbfw_storage_read_multiple_block_begin(
				(void *)gbfw_usb_mass_storage__block(1)))
			{
				gbfw_storage_read_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR);
				return;
			}
		}
		else if ((gbfw_usb_mass_storage__flags.rbuff == 0)
			&& (gbfw_usb_mass_storage__flags.wbuff == 1))
		{
			if (buff == (block0 + (GBFW_STORAGE_BLOCK_SIZE / 2)))
			{
				if (!gbfw_storage_read_multiple_block_end())
				{
					gbfw_storage_read_multiple_end();
					gbfw_usb_mass_storage__scsi_error(
						SCSI_SENSE_KEY_MEDIUM_ERROR,
						SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR);
					return;
				}
			}
			else if ((buff == (block0 + GBFW_STORAGE_BLOCK_SIZE))
				&& (transfer_length > (GBFW_STORAGE_BLOCK_SIZE / 2)))
			{
				gbfw_usb_mass_storage__flags.rbuff = 1;
				gbfw_usb_mass_storage__flags.wbuff = 0;

				if (!gbfw_storage_read_multiple_block_begin(
					(void *)gbfw_usb_mass_storage__block(0)))
				{
					gbfw_storage_read_multiple_end();
					gbfw_usb_mass_storage__scsi_error(
						SCSI_SENSE_KEY_MEDIUM_ERROR,
						SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR);
					return;
				}
			}
		}
		else if ((gbfw_usb_mass_storage__flags.rbuff == 1)
			&& (buff == (block1 + GBFW_STORAGE_BLOCK_SIZE)))
		{
			if (!gbfw_storage_read_multiple_block_end())
			{
				gbfw_storage_read_multiple_end();
				gbfw_usb_mass_storage__scsi_error(
					SCSI_SENSE_KEY_MEDIUM_ERROR,
					SCSI_SENSE_QUALIFIER_UNRECOVERED_READ_ERROR);
				return;
			}

			buff = block0;
			gbfw_usb_mass_storage__flags.rbuff = 0;
		}

		gbfw_usb_endpoint_set_packet(ep, buff, size);
	}
}

static const gbfw_usb_device_t gbfw_usb_mass_storage__device =
{
	.descriptors    = &usb_mass_storage__descriptors,
	.buffer_init    = gbfw_usb_mass_storage__buffer_init,
	.reset          = gbfw_usb_mass_storage__reset,
	.setup          = gbfw_usb_mass_storage__setup,
	.disconnect     = gbfw_usb_mass_storage__disconnect,
	.endpoint_stall = gbfw_usb_mass_storage__endpoint_stall,
	.recv           = gbfw_usb_mass_storage__recv,
	.sent           = gbfw_usb_mass_storage__sent,

	.buffer = GBFW_STORAGE_BLOCK_SIZE + (GBFW_STORAGE_BLOCK_SIZE / 2)
		+ STM32_USB_SRAM_SIZE_ROUND16(sizeof(usb_mass_storage_cbw_t))
		+ STM32_USB_SRAM_SIZE_ROUND16(sizeof(usb_mass_storage_csw_t))
		+ STM32_USB_SRAM_SIZE_ROUND16(sizeof(scsi_request_sense_data_t)),
};

bool gbfw_usb_mass_storage_enable(void)
{
	if (!gbfw_storage_lock())
		return false;

	gbfw_usb_device_register(&gbfw_usb_mass_storage__device);
	return true;
}

void gbfw_usb_mass_storage_disable(void)
{
	gbfw_usb_device_register(NULL);
	gbfw_storage_release();
}
