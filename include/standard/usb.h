#ifndef USB_H
#define USB_H

#include <stdint.h>

typedef enum
{
	USB_DESCRIPTOR_DEVICE           = 0x01,
	USB_DESCRIPTOR_CONFIGURATION    = 0x02,
	USB_DESCRIPTOR_STRING           = 0x03,
	USB_DESCRIPTOR_INTERFACE        = 0x04,
	USB_DESCRIPTOR_ENDPOINT         = 0x05,
	USB_DESCRIPTOR_DEVICE_QUALIFIER = 0x06,
} usb_descriptor_type_e;

typedef enum
{
	USB_SPEC_1_0 = 0x0100,
	USB_SPEC_1_1 = 0x0110,
	USB_SPEC_2_0 = 0x0200,
} usb_spec_e;

typedef enum
{
	USB_CLASS_PER_INTERFACE             = 0x00,
	USB_CLASS_AUDIO                     = 0x01,
	USB_CLASS_CDC_CONTROL               = 0x02,
	USB_CLASS_HID                       = 0x03,
	USB_CLASS_PHYSICAL                  = 0x05,
	USB_CLASS_IMAGE                     = 0x06,
	USB_CLASS_PRINTER                   = 0x07,
	USB_CLASS_MASS_STORAGE              = 0x08,
	USB_CLASS_HUB                       = 0x09,
	USB_CLASS_CDC_DATA                  = 0x0A,
	USB_CLASS_SMART_CARD                = 0x0B,
	USB_CLASS_CONTENT_SECURITY          = 0x0D,
	USB_CLASS_VIDEO                     = 0x0E,
	USB_CLASS_PERSONAL_HEALTHCARE       = 0x0F,
	USB_CLASS_AUDIO_VIDEO               = 0x10,
	USB_CLASS_BILLBOARD                 = 0x11,
	USB_CLASS_USB_TYPE_C_BRIDGE         = 0x12,
	USB_CLASS_USB_BULK_DISPLAY_PROTOCOL = 0x13,
	USB_CLASS_MCTP_OVER_USB             = 0x14,
	USB_CLASS_I3C                       = 0x3C,
	USB_CLASS_DIAGNOSTIC                = 0xDC,
	USB_CLASS_WIRELESS_CONTROLLER       = 0xE0,
	USB_CLASS_MISC                      = 0xEF,
	USB_CLASS_APPLICATION_SPECIFIC      = 0xFE,
	USB_CLASS_VENDOR_SPECIFIC           = 0xFF,
} usb_class_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t  length;
	uint8_t  descriptor_type;
	uint16_t usb_spec;
	uint8_t  device_class;
	uint8_t  device_sub_class;
	uint8_t  device_protcol;
	uint8_t  max_packet_size;
	uint16_t vendor_id;
	uint16_t product_id;
	uint16_t device;
	uint8_t  manufacturer;
	uint8_t  product;
	uint8_t  serial_number;
	uint8_t  num_configurations;
} usb_descriptor_device_t;

#define USB_POWER_MA(x) ((x) > 510 ? 255 : ((x) / 2))

typedef struct
__attribute__((__packed__))
{
	uint8_t  length;
	uint8_t  descriptor_type;
	uint16_t total_length;
	uint8_t  num_interfaces;
	uint8_t  configuration_value;
	uint8_t  configuration;

	union
	{
		struct
		{
			uint8_t res4_0           : 5;
			uint8_t remote_wakeup    : 1;
			uint8_t self_powered     : 1;
			uint8_t usb1_bus_powered : 1;
		};

		uint8_t mask;
	} attributes;

	uint8_t max_power;
} usb_descriptor_configuration_t;

typedef struct
__attribute__((__packed__))
{
	uint8_t length;
	uint8_t descriptor_type;
	uint8_t interface_number;
	uint8_t alternate_setting;
	uint8_t num_endpoints;
	uint8_t interface_class;
	uint8_t interface_sub_class;
	uint8_t interface_protocol;
	uint8_t interface;
} usb_descriptor_interface_t;

typedef enum
{
	USB_TRANSFER_TYPE_CONTROL     = 0,
	USB_TRANSFER_TYPE_ISOCHRONOUS = 1,
	USB_TRANSFER_TYPE_BULK        = 2,
	USB_TRANSFER_TYPE_INTERRUPT   = 3,
} usb_transfer_type_e;

typedef enum
{
	USB_SYNC_TYPE_NONE        = 0,
	USB_SYNC_TYPE_ASYNC       = 1,
	USB_SYNC_TYPE_ADAPTIVE    = 2,
	USB_SYNC_TYPE_SYNCHRONOUS = 3,
} usb_sync_type_e;

typedef enum
{
	USB_USAGE_TYPE_DATA                   = 0,
	USB_USAGE_TYPE_FEEDBACK               = 1,
	USB_USAGE_TYPE_EXPLICIT_FEEDBACK_DATA = 2,
} usb_usage_type_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t length;
	uint8_t descriptor_type;
	uint8_t endpoint_address;

	union
	{
		struct
		{
			uint8_t transfer_type : 2;
			uint8_t sync_type     : 2;
			uint8_t usage_type    : 2;
			uint8_t res7_6        : 2;
		};

		uint8_t mask;
	} attributes;

	uint16_t max_packet_size;
	uint8_t  interval;
} usb_descriptor_endpoint_t;


typedef enum
{
	USB_LANG_ID_ENGLISH_UNITED_STATES = 0x0409,
} usb_lang_id_e;

typedef struct
__attribute__((__packed__))
{
	uint8_t  length;
	uint8_t  descriptor_type;
	uint16_t string[];
} usb_descriptor_string_t;


typedef enum
{
	USB_REQUEST_GET_STATUS        = 0x00,
	USB_REQUEST_CLEAR_FEATURE     = 0x01,
	USB_REQUEST_SET_FEATURE       = 0x03,
	USB_REQUEST_SET_ADDRESS       = 0x05,
	USB_REQUEST_GET_DESCRIPTOR    = 0x06,
	USB_REQUEST_SET_DESCRIPTOR    = 0x07,
	USB_REQUEST_GET_CONFIGURATION = 0x08,
	USB_REQUEST_SET_CONFIGURATION = 0x09,
	USB_REQUEST_GET_INTERFACE     = 0x0A,
	USB_REQUEST_SET_INTERFACE     = 0x0B,
	USB_REQUEST_SYNCH_FRAME       = 0x0C,
} usb_request_e;

typedef enum
{
	USB_REQUEST_DIRECTION_HOST_TO_DEVICE = 0,
	USB_REQUEST_DIRECTION_DEVICE_TO_HOST = 1,
} usb_request_direction_e;

typedef enum
{
	USB_REQUEST_TYPE_STANDARD = 0,
	USB_REQUEST_TYPE_CLASS    = 1,
	USB_REQUEST_TYPE_VENDOR   = 2,
} usb_request_type_e;

typedef enum
{
	USB_REQUEST_RECIPIENT_DEVICE    = 0,
	USB_REQUEST_RECIPIENT_INTERFACE = 1,
	USB_REQUEST_RECIPIENT_ENDPOINT  = 2,
	USB_REQUEST_RECIPIENT_OTHER     = 3,
} usb_request_recipient_e;

typedef struct
__attribute__((__packed__))
{
	union
	{
		struct
		{
			uint8_t recipient : 5;
			uint8_t type      : 2;
			uint8_t direction : 1;
		};

		uint8_t mask;
	} request_type;

	uint8_t  request;
	uint16_t value;
	uint16_t index;
	uint16_t length;
} usb_request_t;

#endif
