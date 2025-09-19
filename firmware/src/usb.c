#include "usb.h"
#include "task.h"
#include "analog.h"
#include "battery.h"
#include "delay.h"
#include "irq.h"
#include <hardware/stm32/rcc.h>
#include <hardware/stm32/crs.h>
#include <hardware/stm32/usb.h>
#include <hardware/stm32/gpio.h>
#include <hardware/stm32/syscfg.h>
#include <hardware/stm32/exti.h>
#include <hardware/stm32/irq.h>
#include <hardware/stm32/device.h>
#include <hardware/cm4/nvic.h>
#include <hardware/cm4/intrinsics.h>
#include <standard/usb.h>
#include <stddef.h>


#if (REVISION == 5)
#define GBFW_USB_DETECT_PORT       STM32_GPIO_PORT_B
#define GBFW_USB_DETECT_PIN        11
#define GBFW_USB_DETECT_IRQ_VECTOR stm32_vector_irq_exti_11
#elif (REVISION <= 7)
#define GBFW_USB_DETECT_PORT       STM32_GPIO_PORT_F
#define GBFW_USB_DETECT_PIN        0
#define GBFW_USB_DETECT_IRQ_VECTOR stm32_vector_irq_exti_0
#else
#define GBFW_USB_DETECT_PORT       STM32_GPIO_PORT_A
#define GBFW_USB_DETECT_PIN        0
#define GBFW_USB_DETECT_IRQ_VECTOR stm32_vector_irq_exti_0
#endif


typedef union
{
	struct
	{
		uint8_t data        : 1; // Host data connection available.
		uint8_t power       : 1; // Port is powered externally.
		uint8_t fast_charge : 1; // 500mA+ charging supported.
		uint8_t link        : 1; // USB port being used as link cable.
		uint8_t configured  : 1;
		uint8_t res7_5      : 3;
	};

	uint8_t mask;
} gbfw_usb_state_t;


static volatile gbfw_usb_state_t gbfw_usb__state = { .mask = 0x00 };
static const gbfw_usb_device_t *gbfw_usb__device = NULL;


static const usb_descriptor_string_t gbfw_usb__descriptor_string_zero =
{
	.length          = 4,
	.descriptor_type = USB_DESCRIPTOR_STRING,
	.string          = { USB_LANG_ID_ENGLISH_UNITED_STATES },
};


static inline const usb_descriptor_device_t *gbfw_usb__descriptor_device(void)
{
	return (const usb_descriptor_device_t *)gbfw_usb__device->descriptors;
}

// We only support a single configuration descriptor to save space and simplify.
static inline const usb_descriptor_configuration_t *gbfw_usb__descriptor_configuration(void)
{
	return (const usb_descriptor_configuration_t *)(
		(uintptr_t)gbfw_usb__device->descriptors
		+ sizeof(usb_descriptor_device_t));
}

// We only support a single interface descriptor to save space and simplify.
static inline const usb_descriptor_interface_t *gbfw_usb__descriptor_interface(void)
{
	return (const usb_descriptor_interface_t *)(
		(uintptr_t)gbfw_usb__descriptor_configuration() + sizeof(usb_descriptor_configuration_t));
}

static inline const usb_descriptor_endpoint_t *gbfw_usb__descriptor_endpoint(uint8_t index)
{
	return (const usb_descriptor_endpoint_t *)(
		(uintptr_t)gbfw_usb__descriptor_interface() + sizeof(usb_descriptor_interface_t)
		+ (index * sizeof(usb_descriptor_endpoint_t)));
}

static inline uint16_t gbfw_usb__descriptor_string_serial_ptr(void)
{
	return gbfw_usb__device->buffer;
}

static inline const usb_descriptor_string_t *gbfw_usb__descriptor_string(uint8_t index)
{
	if (index == 0)
	{
		return &gbfw_usb__descriptor_string_zero;
	}

	// This will return the wrong value if called with index 1.
	// Since string 1 (serial) is dynamic it's stored in packet ram.

	const usb_descriptor_configuration_t *configuration
		= gbfw_usb__descriptor_configuration();
	const usb_descriptor_string_t *string = (const usb_descriptor_string_t *)(
		(uintptr_t)configuration + configuration->total_length);

	for (; index > 2; index--)
	{
		string = (const usb_descriptor_string_t *)(
			(uintptr_t)string + string->length);
	}

	return string;
}



static void gbfw_usb_device__initialize_sram(void)
{
	// Initialize device specific buffers.
	gbfw_usb__device->buffer_init();

	uint16_t ptr = gbfw_usb__device->buffer;

	// Serial String Descriptor
	{
		// Generate Serial Number Descriptor from Device ID.
		volatile usb_descriptor_string_t *serial = STM32_USB_SRAM_PTR(ptr);

		// Pack these bytes as USB SRAM requires 16-bit writes.
		uint8_t serial_len = 50;
		uint16_t length_descriptor_type = (USB_DESCRIPTOR_STRING << 8) | serial_len;
		stm32_usb_sram_write16(STM32_USB_SRAM_PTR(ptr), length_descriptor_type);

		static const char hex[16] = "0123456789ABCDEF";
		for (uint8_t i = 0; i < 24; i++)
		{
			uint32_t w = STM32_UID[i / 8];
			uint32_t n = (w >> ((i % 8) * 4)) & 0xF;
			// We can get away with this as we know the string is always 2-byte aligned.
			serial->string[i] = hex[n];
		}
		ptr += serial_len;
	}

	const usb_descriptor_interface_t *interface
		= gbfw_usb__descriptor_interface();

	// BTABLE must be aligned to 8-bytes.
	if (ptr % 8) ptr += 8 - (ptr % 8);

	stm32_usb->btable = ptr;
	volatile stm32_usb_bdt_t * const bdt = STM32_USB_SRAM_PTR(ptr);
	ptr += (interface->num_endpoints + 1) * sizeof(stm32_usb_bdt_t);

	// SETUP Endpoint
	{
		size_t size = 64;
		stm32_usb_bdt_count_t count = stm32_usb_bdt_count_encode(size);

		bdt[0].addr_rx       = ptr;
		bdt[0].count_rx.mask = count.mask;

		bdt[0].addr_tx  = 0;
		bdt[0].count_tx = 0;
	}

	for (uint8_t i = 1; i <= interface->num_endpoints; i++)
	{
		bdt[i].addr_rx       = 0;
		bdt[i].count_rx.mask = 0;
		bdt[i].addr_tx       = 0;
		bdt[i].count_tx      = 0;
	}
}

static void gbfw_usb_device__reset(void)
{
	const usb_descriptor_interface_t *interface
		= gbfw_usb__descriptor_interface();

	// SETUP Endpoint
	{
		stm32_usb_ep_t epr = stm32_usb->ep[0];
		epr.ea      = 0;
		epr.stat_tx ^= STM32_USB_EP_STAT_NAK;
		epr.dtog_tx = 0;
		epr.ctr_tx  = 0;
		epr.ep_kind = STM32_USB_EP_KIND_DEFAULT;
		epr.ep_type = STM32_USB_EP_TYPE_CONTROL;
		epr.setup   = 0;
		epr.stat_rx ^= STM32_USB_EP_STAT_VALID;
		epr.dtog_rx = 0;
		epr.ctr_rx  = 0;
		stm32_usb->ep[0] = epr;
	}

	static const stm32_usb_ep_type_e type_map[4] =
	{
		[USB_TRANSFER_TYPE_CONTROL    ] = STM32_USB_EP_TYPE_CONTROL,
		[USB_TRANSFER_TYPE_ISOCHRONOUS] = STM32_USB_EP_TYPE_ISO,
		[USB_TRANSFER_TYPE_BULK       ] = STM32_USB_EP_TYPE_BULK,
		[USB_TRANSFER_TYPE_INTERRUPT  ] = STM32_USB_EP_TYPE_INTERRUPT,
	};

	for (uint8_t i = 1; i <= interface->num_endpoints; i++)
	{
		const usb_descriptor_endpoint_t *endpoint
			= gbfw_usb__descriptor_endpoint(i - 1);

		bool    in   = endpoint->endpoint_address >> 7;
		bool    out  = !in;
		uint8_t addr = endpoint->endpoint_address & 0x0F;

		stm32_usb_ep_type_e type = type_map[endpoint->attributes.transfer_type];
		stm32_usb_ep_kind_e kind = STM32_USB_EP_KIND_DEFAULT;

		stm32_usb_ep_t ep = stm32_usb->ep[i];
		ep.ea      = addr;
		ep.stat_tx ^= (in ? STM32_USB_EP_STAT_NAK : STM32_USB_EP_STAT_DISABLED);
		// Leave dtog_tx at current value to reset it to zero.
		ep.ctr_tx  = 0;
		ep.ep_kind = kind;
		ep.ep_type = type;
		ep.setup   = 0;
		ep.stat_rx ^= (out ? STM32_USB_EP_STAT_NAK : STM32_USB_EP_STAT_DISABLED);
		// Leave dtog_rx at current value to reset it to zero.
		ep.ctr_rx  = 0;
		stm32_usb->ep[i] = ep;
	}

	stm32_usb_dadd_t dadd = { .ef = true, .add = 0x00 };
	stm32_usb->dadd.mask = dadd.mask;

	gbfw_usb__device->reset();
}

void gbfw_usb_device__enable(void)
{
	stm32_usb->cnt.pdwn = false;
	// Missing in datasheet so assumed based on forum response for H series chip.
	gbfw_delay_us(1);
	stm32_usb->cnt.fres = false;
	stm32_usb->ist.mask = 0;

	gbfw_usb_device__reset();

	stm32_usb->cnt.ctrm   = true;
	stm32_usb->cnt.resetm = true;

	stm32_usb->bcd.dppu = true;
}

void gbfw_usb_device__disable(void)
{
	if (gbfw_usb__device)
		gbfw_usb__device->disconnect();

	stm32_usb->bcd.dppu = false;

	stm32_usb->cnt.fres = true;
	stm32_usb->cnt.pdwn = true;

	gbfw_usb__state.configured = false;
}

void gbfw_usb_device_register(const gbfw_usb_device_t *device)
{
	gbfw_usb__device = device;

	if (gbfw_usb__device)
	{
		gbfw_usb_device__initialize_sram();

		if (gbfw_usb__state.data)
			gbfw_usb_device__enable();
	}
	else
	{
		gbfw_usb_device__disable();
	}
}



static void gbfw_usb_connect_phase(void)
{
	if (stm32_usb->bcd.pden)
	{
		gbfw_usb__state.fast_charge = stm32_usb->bcd.pdet;
		gbfw_usb__state.data = !gbfw_usb__state.fast_charge;
		stm32_usb->bcd.pden = false;

		if (gbfw_usb__state.fast_charge)
		{
			stm32_usb->bcd.sden = true;
			gbfw_task_schedule(gbfw_usb_connect_phase, 50000);
			return;
		}
	}
	else if (stm32_usb->bcd.sden)
	{
		gbfw_usb__state.data = !stm32_usb->bcd.sdet;
		stm32_usb->bcd.sden = false;

		gbfw_battery_fast_charge(true);
	}

	stm32_usb->bcd.bcden = false;

	if (gbfw_usb__state.data && gbfw_usb__device)
		gbfw_usb_device__enable();
}

static void gbfw_usb_connect(void)
{
	gbfw_usb__state.power = true;
	gbfw_usb__state.link  = false;

	stm32_usb->bcd.bcden = true;
	stm32_usb->bcd.pden  = true;
	gbfw_task_schedule(gbfw_usb_connect_phase, 50000);
}

static void gbfw_usb_disconnect(void)
{
	gbfw_usb_device__disable();

	gbfw_battery_fast_charge(false);

	gbfw_usb__state.power       = false;
	gbfw_usb__state.fast_charge = false;
	gbfw_usb__state.data        = false;
}

void __attribute__((interrupt)) GBFW_USB_DETECT_IRQ_VECTOR(void)
{
	stm32_exti_p_clear(GBFW_USB_DETECT_PIN);

	bool power = stm32_gpio_pin_read(GBFW_USB_DETECT_PORT, GBFW_USB_DETECT_PIN);
	if (power != gbfw_usb__state.power)
	{
		if (power)
			gbfw_usb_connect();
		else
			gbfw_usb_disconnect();
	}

	// STM32 Errata 2.1.3
	dsb();
}


static inline bool gbfw_usb__ep0_setup_device_request(
	volatile const usb_request_t *request,
	volatile uint16_t *addr, uint16_t *len)
{
	bool success = false;
	switch (request->request)
	{
		case USB_REQUEST_GET_STATUS:
			*len = 2;
			*addr = (uintptr_t)request & 0x03FF;
			*((volatile uint16_t *)request) = 0x0000;
			success = true;
			break;

		case USB_REQUEST_SET_ADDRESS:
			// Address is set after the status stage completes.
			success = true;
			break;

		case USB_REQUEST_GET_DESCRIPTOR:
		{
			uint8_t               index = request->value & 0xFF;
			usb_descriptor_type_e type  = request->value >> 8;

			switch (type)
			{
				case USB_DESCRIPTOR_DEVICE:
					if (index == 0)
					{
						const usb_descriptor_device_t *device
							= gbfw_usb__descriptor_device();
						uint16_t buff = (uintptr_t)request & 0x03FF;
						uint8_t  size = sizeof(usb_descriptor_device_t);

						stm32_usb_sram_copy_in(buff, device, size);
						*addr = buff;
						*len  = size;
						success = true;
					}
					break;

				case USB_DESCRIPTOR_CONFIGURATION:
					if (index == 0)
					{
						const usb_descriptor_configuration_t *configuration
							= gbfw_usb__descriptor_configuration();
						uint16_t buff = (uintptr_t)request & 0x03FF;
						uint8_t  size = configuration->total_length;

						stm32_usb_sram_copy_in(buff, configuration, size);
						*addr = buff;
						*len  = size;
						success = true;
					}
					break;

				case USB_DESCRIPTOR_STRING:
					if (index == 1)
					{
						uint16_t buff = gbfw_usb__descriptor_string_serial_ptr();
						*addr = buff;
						*len = ((volatile const usb_descriptor_string_t *)STM32_USB_SRAM_PTR(buff))->length;
						success = true;
					}
					else
					{
						const usb_descriptor_string_t *string
							= gbfw_usb__descriptor_string(index);
						uint16_t buff = (uintptr_t)request & 0x03FF;
						uint8_t  size = string->length;

						stm32_usb_sram_copy_in(buff, string, size);
						*addr = buff;
						*len  = size;
						success = true;
					}
					break;

				default:
					break;
			}
		} break;

		case USB_REQUEST_GET_CONFIGURATION:
			*len = 2;
			*addr = (uintptr_t)request & 0x03FF;
			*((volatile uint16_t *)request) = gbfw_usb__state.configured;
			success = true;
			break;

		case USB_REQUEST_SET_CONFIGURATION:
			gbfw_usb__state.configured = (request->value == 1);
			if (gbfw_usb__state.configured && !gbfw_usb__state.fast_charge
				&& gbfw_usb__descriptor_configuration()->max_power >= USB_POWER_MA(500))
			{
				gbfw_usb__state.fast_charge = true;
				gbfw_battery_fast_charge(true);
			}
			success = true;
			break;

		default:
			break;

	}

	return success;
}

static inline bool gbfw_usb__ep0_setup_endpoint_request(
	volatile const usb_request_t *request,
	volatile uint16_t *addr, uint16_t *len)
{
	bool success = false;

	stm32_usb_ep_stat_e stat = STM32_USB_EP_STAT_DISABLED;
	uint8_t ep;
	for (ep = 1; ep < STM32_USB_ENDPOINTS; ep++)
	{
		stm32_usb_ep_t epr = stm32_usb->ep[ep];

		if (epr.ea != (request->index & 0x7F))
			continue;

		stat = ((request->index >> 7)
			? epr.stat_tx : epr.stat_rx);

		if (stat != STM32_USB_EP_STAT_DISABLED)
			break;
	}
	if (stat == STM32_USB_EP_STAT_DISABLED) return false;

	bool stall = (stat == STM32_USB_EP_STAT_STALL);

	switch (request->request)
	{
		case USB_REQUEST_GET_STATUS:
			*len = 2;
			*addr = (uintptr_t)request & 0x03FF;
			*((volatile uint16_t *)request) = stall;
			success = true;
			break;

		case USB_REQUEST_CLEAR_FEATURE:
			if (ep == 0)
				success = false;
			else if (!stall)
				success = true;
			else
				success = gbfw_usb__device->endpoint_stall(ep, false);
			break;

		case USB_REQUEST_SET_FEATURE:
			if (ep == 0)
				success = false;
			else if (stall)
				success = true;
			else
				success = gbfw_usb__device->endpoint_stall(ep, true);
			break;

		default:
			break;

	}

	return success;
}

static void gbfw_usb__ep0_setup_received(void)
{
	volatile stm32_usb_bdt_t *bdt = STM32_USB_SRAM_PTR(stm32_usb->btable);
	volatile usb_request_t *request = STM32_USB_SRAM_PTR(bdt[0].addr_rx);

	// We need to read request length here as it may be overwitten by
	// the endpoint handlers.
	uint16_t request_length = request->length;

	uint16_t len = 0;
	bool success;
	if (request->request_type.type == USB_REQUEST_TYPE_STANDARD)
	{
		switch (request->request_type.recipient)
		{
			case USB_REQUEST_RECIPIENT_DEVICE:
				success = gbfw_usb__ep0_setup_device_request(
					request, &bdt[0].addr_tx, &len);
				break;

			// None of the interface requests do anything useful.

			case USB_REQUEST_RECIPIENT_ENDPOINT:
				success = gbfw_usb__ep0_setup_endpoint_request(
					request, &bdt[0].addr_tx, &len);
				break;

			default:
				success = false;
				break;
		}
	}
	else
	{
		success = gbfw_usb__device->setup(
			request, &bdt[0].addr_tx, &len);
	}

	if (len > request_length)
		len = request_length;
	bdt[0].count_tx = len;

	stm32_usb_ep_t ep = stm32_usb->ep[0];
	ep.dtog_rx = 0;
	ep.dtog_tx = 0;
	ep.ctr_rx  = 0;

	if (success)
	{
		ep.stat_rx ^= (len == 0
			? STM32_USB_EP_STAT_NAK
			: STM32_USB_EP_STAT_STALL);
		ep.stat_tx ^= STM32_USB_EP_STAT_VALID;
	}
	else
	{
		ep.stat_rx ^= STM32_USB_EP_STAT_VALID;
		ep.stat_tx ^= STM32_USB_EP_STAT_STALL;
	}

	stm32_usb->ep[0] = ep;
}

static void gbfw_usb__ep0_data_complete(void)
{
	stm32_usb_ep_t ep = stm32_usb->ep[0];
	ep.dtog_rx = 0;
	ep.dtog_tx = 0;
	ep.ctr_tx  = 0;
	ep.ep_kind = STM32_USB_EP_KIND_STATUS_OUT;
	ep.stat_rx ^= STM32_USB_EP_STAT_VALID;
	ep.stat_tx ^= STM32_USB_EP_STAT_NAK;
	stm32_usb->ep[0] = ep;
}

static void gbfw_usb__ep0_status_complete(void)
{
	stm32_usb_ep_t ep = stm32_usb->ep[0];

	// Reading the untouched RX buffer request saves us a byte of SRAM here.
	if (ep.ep_kind != STM32_USB_EP_KIND_STATUS_OUT)
	{
		volatile stm32_usb_bdt_t *bdt
			= STM32_USB_SRAM_PTR(stm32_usb->btable);
		volatile usb_request_t *request
			= STM32_USB_SRAM_PTR(bdt[0].addr_rx);
		if (request->request == USB_REQUEST_SET_ADDRESS)
			stm32_usb->dadd.add = (request->value & 0x7F);
	}

	ep.dtog_rx = 0;
	ep.dtog_tx = 0;
	ep.ctr_tx  = 0;
	ep.ctr_rx  = 0;
	ep.ep_kind = STM32_USB_EP_KIND_DEFAULT;
	ep.stat_rx ^= STM32_USB_EP_STAT_VALID;
	ep.stat_tx ^= STM32_USB_EP_STAT_NAK;
	stm32_usb->ep[0] = ep;
}

static void gbfw_usb__ep0_handler(void)
{
	if (stm32_usb->ep[0].ctr_tx)
	{
		volatile stm32_usb_bdt_t *bdt = STM32_USB_SRAM_PTR(stm32_usb->btable);
		if (bdt[0].count_tx == 0)
			gbfw_usb__ep0_status_complete();
		else
			gbfw_usb__ep0_data_complete();
	}

	if (stm32_usb->ep[0].ctr_rx)
	{
		if (stm32_usb->ep[0].setup)
			gbfw_usb__ep0_setup_received();
		else
			gbfw_usb__ep0_status_complete();
	}
}



void __attribute__((interrupt)) stm32_vector_irq_usb_lp(void)
{
	if (stm32_usb->ist.reset)
	{
		gbfw_usb_device__reset();
		stm32_usb->ist.reset = false;
	}

	if (stm32_usb->ist.ctr)
	{
		uint8_t id = stm32_usb->ist.ep_id;

		if (id == 0)
		{
			gbfw_usb__ep0_handler();
		}
		else
		{
			volatile stm32_usb_bdt_t *bdt
				= STM32_USB_SRAM_PTR(stm32_usb->btable);

			stm32_usb_ep_t ep = stm32_usb->ep[id];
			ep.dtog_rx = 0;
			ep.dtog_tx = 0;
			ep.stat_rx = 0;
			ep.stat_tx = 0;

			if (ep.ctr_tx)
			{
				stm32_usb_ep_t epc = ep;
				epc.ctr_rx = 1;
				epc.ctr_tx = 0;
				stm32_usb->ep[id] = epc;

				gbfw_usb__device->sent(id,
					bdt[id].addr_tx,
					bdt[id].count_tx);
			}

			if (ep.ctr_rx)
			{
				stm32_usb_ep_t epc = ep;
				epc.ctr_tx = 1;
				epc.ctr_rx = 0;
				stm32_usb->ep[id] = epc;

				gbfw_usb__device->recv(id,
					bdt[id].addr_rx,
					bdt[id].count_rx.count);
			}
		}
	}

	// STM32 Errata 2.1.3
	dsb();
}

void gbfw_usb_init(void)
{
	stm32_rcc->apb1en1.usb = true;

	stm32_gpio_pin_mode_set(GBFW_USB_DETECT_PORT, GBFW_USB_DETECT_PIN, STM32_GPIO_MODE_INPUT);

	stm32_syscfg_exti_port_set(GBFW_USB_DETECT_PIN, GBFW_USB_DETECT_PORT);
	gbfw_irq_enable(GBFW_USB_DETECT_PIN, true, true, GBFW_IRQ_PRIORITY_USB_DETECT);

	// Trigger interrupt to determine initial state.
	stm32_exti_swie_set(GBFW_USB_DETECT_PIN);

	cm4_nvic_interrupt_priority_set(STM32_IRQ_USB_LP, GBFW_IRQ_PRIORITY_USB);
	cm4_nvic_interrupt_enable(STM32_IRQ_USB_LP);
}

uint16_t gbfw_usb_voltage(void)
{
	stm32_exti_im_clear(GBFW_USB_DETECT_PIN);

	stm32_gpio_pin_mode_set(GBFW_USB_DETECT_PORT, GBFW_USB_DETECT_PIN, STM32_GPIO_MODE_ANALOG);

	uint16_t voltage = gbfw_analog_usb_voltage();

	stm32_exti_im_set(GBFW_USB_DETECT_PIN);

	// Trigger interrupt to determine current state in-case it changed during voltage read.
	stm32_exti_swie_set(GBFW_USB_DETECT_PIN);

	return voltage;
}

void gbfw_usb_endpoint_stall(uint8_t index)
{
	stm32_usb_ep_t ep = stm32_usb->ep[index];
	ep.dtog_rx = 0;
	ep.dtog_tx = 0;
	ep.ctr_rx  = 1;
	ep.ctr_tx  = 1;

	// No change needed as disabled is zero.
	if (ep.stat_rx != STM32_USB_EP_STAT_DISABLED)
		ep.stat_rx ^= STM32_USB_EP_STAT_STALL;

	// No change needed as disabled is zero.
	if (ep.stat_tx != STM32_USB_EP_STAT_DISABLED)
		ep.stat_tx ^= STM32_USB_EP_STAT_STALL;

	stm32_usb->ep[index] = ep;
}

void gbfw_usb_endpoint_set_packet(uint8_t index, uint16_t buff, uint16_t size)
{
	stm32_usb_ep_t ep = stm32_usb->ep[index];
	ep.ctr_rx  = 1;
	ep.ctr_tx  = 1;

	volatile stm32_usb_bdt_t * const bdt
		= STM32_USB_SRAM_PTR(stm32_usb->btable);
	if (ep.stat_tx != STM32_USB_EP_STAT_DISABLED)
	{
		bdt[index].addr_tx  = buff;
		bdt[index].count_tx = size;

		ep.dtog_rx = 0;

		// Set DTOG to zero on stall clear by leaving unchanged.
		if (ep.stat_tx != STM32_USB_EP_STAT_STALL)
			ep.dtog_tx = 0;

		ep.stat_tx ^= (size > 0
			? STM32_USB_EP_STAT_VALID
			: STM32_USB_EP_STAT_NAK);
		// Don't set stat_rx as interface endpoints are unidirectional.
	}
	else
	{
		stm32_usb_bdt_count_t count = stm32_usb_bdt_count_encode(
			stm32_usb_bdt_size_next(size));

		bdt[index].addr_rx  = buff;
		bdt[index].count_rx = count;

		ep.dtog_tx = 0;

		// Set DTOG to zero on stall clear by leaving unchanged.
		if (ep.stat_rx != STM32_USB_EP_STAT_STALL)
			ep.dtog_rx = 0;

		ep.stat_rx ^= (size > 0
			? STM32_USB_EP_STAT_VALID
			: STM32_USB_EP_STAT_NAK);
	}

	stm32_usb->ep[index] = ep;
}
