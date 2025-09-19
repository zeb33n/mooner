#ifndef USB_MASS_STORAGE_H
#define USB_MASS_STORAGE_H

#include <stdint.h>

typedef enum
{
	USB_SUB_CLASS_MASS_STORAGE_SCSI             = 0x00,
	USB_SUB_CLASS_MASS_STORAGE_RBC              = 0x01,
	USB_SUB_CLASS_MASS_STORAGE_MMC_5            = 0x02,
	USB_SUB_CLASS_MASS_STORAGE_QIC_157          = 0x03,
	USB_SUB_CLASS_MASS_STORAGE_UFI              = 0x04,
	USB_SUB_CLASS_MASS_STORAGE_SFF_8070I        = 0x05,
	USB_SUB_CLASS_MASS_STORAGE_SCSI_TRANSPARENT = 0x06,
	USB_SUB_CLASS_MASS_STORAGE_LSD_FS           = 0x07,
	USB_SUB_CLASS_MASS_STORAGE_IEEE_1667        = 0x08,
} usb_sub_class_mass_storage_e;

typedef enum
{
	USB_PROTOCOL_MASS_STORAGE_CBI       = 0x00,
	USB_PROTOCOL_MASS_STORAGE_CBI_NOCCI = 0x01,
	USB_PROTOCOL_MASS_STORAGE_BBB       = 0x50,
	USB_PROTOCOL_MASS_STORAGE_UAS       = 0x62,
} usb_protocol_mass_storage_e;

typedef enum
{
	USB_REQUEST_MASS_STORAGE_ADSC  = 0x00,
	USB_REQUEST_MASS_STORAGE_GML   = 0xFE,
	USB_REQUEST_MASS_STORAGE_BOMSR = 0xFF,
} usb_request_mass_storage_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t signature;
	uint32_t tag;
	uint32_t data_transfer_length;

	union
	{
		struct
		{
			uint8_t res5_0    : 6;
			uint8_t zero      : 1;
			uint8_t direction : 1;
		};

		uint8_t mask;
	} flags;

	uint8_t lun;
	uint8_t length;

	uint8_t command_block[16];
} usb_mass_storage_cbw_t;

#define USB_MASS_STORAGE_CBW_SIGNATURE 0x43425355UL

typedef enum
{
	USB_MASS_STORAGE_CSW_STATUS_COMMAND_PASSED = 0x00,
	USB_MASS_STORAGE_CSW_STATUS_COMMAND_FAILED = 0x01,
	USB_MASS_STORAGE_CSW_STATUS_PHASE_ERROR    = 0x02,
} usb_mass_storage_csw_status_e;

typedef struct
__attribute__((__packed__))
{
	uint32_t signature;
	uint32_t tag;
	uint32_t data_residue;
	uint8_t  status;
} usb_mass_storage_csw_t;

#define USB_MASS_STORAGE_CSW_SIGNATURE 0x53425355UL

#endif
