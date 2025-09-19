#ifndef GBFW_USB_H
#define GBFW_USB_H

#include <stdbool.h>
#include <stdint.h>
#include <standard/usb.h>


typedef struct
{
	const void *descriptors;

	void (*buffer_init)(void);
	void (*reset)(void);
	bool (*setup)(volatile const usb_request_t *request, volatile uint16_t *addr, uint16_t *len);
	void (*disconnect)(void);
	bool (*endpoint_stall)(uint8_t ep, bool stall);
	void (*recv)(uint8_t ep, uint16_t buff, uint16_t size);
	void (*sent)(uint8_t ep, uint16_t buff, uint16_t size);

	uint16_t buffer;
} gbfw_usb_device_t;


void gbfw_usb_device_register(const gbfw_usb_device_t *device);

void gbfw_usb_endpoint_stall(uint8_t index);
void gbfw_usb_endpoint_set_packet(uint8_t index, uint16_t buff, uint16_t size);

#endif
