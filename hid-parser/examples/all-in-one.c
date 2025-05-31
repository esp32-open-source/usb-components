#include "hid_parser.h"
#include <stdio.h>

const uint8_t hid_report_descriptor_mouse[] = {
	0x05, 0x01, // Usage Page (Generic Desktop)
	0x09, 0x02, // Usage (Mouse)
	0xA1, 0x01, // Collection (Application)
	0x85, 0x01, //   Report ID (1)
	0x09, 0x01, //   Usage (Pointer)
	0xA1, 0x00, //   Collection (Physical)

	0x05, 0x09, //     Usage Page (Buttons)
	0x19, 0x01, //     Usage Minimum (Button 1)
	0x29, 0x03, //     Usage Maximum (Button 3)
	0x15, 0x00, //     Logical Minimum (0)
	0x25, 0x01, //     Logical Maximum (1)
	0x95, 0x03, //     Report Count (3)
	0x75, 0x01, //     Report Size (1)
	0x81, 0x02, //     Input (Data, Variable, Absolute) ; 3 buttons

	0x95, 0x01, //     Report Count (1)
	0x75, 0x05, //     Report Size (5)
	0x81, 0x03, //     Input (Constant) ; Padding

	0x05, 0x01, //     Usage Page (Generic Desktop)
	0x09, 0x30, //     Usage (X)
	0x09, 0x31, //     Usage (Y)
	0x09, 0x38, //     Usage (Wheel)
	0x15, 0x81, //     Logical Minimum (-127)
	0x25, 0x7F, //     Logical Maximum (127)
	0x75, 0x08, //     Report Size (8)
	0x95, 0x03, //     Report Count (3)
	0x81, 0x06, //     Input (Data, Variable, Relative)

	0xC0, //   End Collection (Physical)
	0xC0  // End Collection (Application)
};

const uint8_t hid_report_descriptor_keyboard[] = {
	// === Collection: Keyboard ===
	0x05, 0x01, // Usage Page (Generic Desktop)
	0x09, 0x06, // Usage (Keyboard)
	0xA1, 0x01, // Collection (Application)
	0x85, 0x04, //   Report ID (1)

	0x05, 0x07, //   Usage Page (Key Codes)
	0x19, 0xE0, //   Usage Minimum (224)
	0x29, 0xE7, //   Usage Maximum (231)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x01, //   Logical Maximum (1)
	0x75, 0x01, //   Report Size (1)
	0x95, 0x08, //   Report Count (8)
	0x81, 0x02, //   Input (Data, Variable, Absolute) ; Modifier keys

	0x95, 0x01, //   Report Count (1)
	0x75, 0x08, //   Report Size (8)
	0x81, 0x03, //   Input (Constant) ; Reserved

	0x95, 0x06, //   Report Count (6)
	0x75, 0x08, //   Report Size (8)
	0x15, 0x00, //   Logical Minimum (0)
	0x25, 0x65, //   Logical Maximum (101)
	0x05, 0x07, //   Usage Page (Key Codes)
	0x19, 0x00, //   Usage Minimum (0)
	0x29, 0x65, //   Usage Maximum (101)
	0x81, 0x00, //   Input (Data, Array)

	0xC0, // End Collection

	// === Collection: Media Keys (Consumer Control) ===
	0x05, 0x0C, // Usage Page (Consumer Devices)
	0x09, 0x01, // Usage (Consumer Control)
	0xA1, 0x01, // Collection (Application)
	0x85, 0x03, //   Report ID (2)

	0x15, 0x00,		  //   Logical Minimum (0)
	0x25, 0x01,		  //   Logical Maximum (1)
	0x75, 0x01,		  //   Report Size (1)
	0x95, 0x07,		  //   Report Count (7)
	0x0A, 0x23, 0x02, //   Usage (Mute)
	0x0A, 0x24, 0x02, //   Usage (Volume Up)
	0x0A, 0x25, 0x02, //   Usage (Volume Down)
	0x0A, 0xB5, 0x00, //   Usage (Next Track)
	0x0A, 0xB6, 0x00, //   Usage (Previous Track)
	0x0A, 0xB7, 0x00, //   Usage (Stop)
	0x0A, 0xCD, 0x00, //   Usage (Play/Pause)
	0x81, 0x02,		  //   Input (Data, Variable, Absolute)

	0x95, 0x01, //   Report Count (1)
	0x75, 0x01, //   Report Size (1)
	0x81, 0x03, //   Input (Constant) ; Padding

	0xC0 // End Collection
};
const char *get_usage_name(uint16_t usage_id);

void mouse_event_handler(const HidFieldInfo *field, uint8_t buttons, int16_t xAxis, int16_t yAxis, int16_t wheel)
{
	printf("Mouse callback\n");
	printf("\tbuttons: %d, X axis: %d, Y axis: %d, Wheel: %d\n", buttons, xAxis, yAxis, wheel);
}

void my_test_callback(const HidFieldInfo *field, const uint8_t *value, size_t len, uint8_t report_id)
{
	printf("Report ID: 0x%02X\n", report_id);
	if (field->name)
		printf("  Name: %s\n", field->name);
	printf("  Usage Page: 0x%02X\n", field->usage_page);
	printf("  Usage ID: 0x%02X\n", field->usage_ids[0]);
	printf("  Offset: %d bits\n", field->offset);
	printf("  Size: %d bits\n", field->size);
	printf("  Count: %u\n", field->count);
	if (!field->flags.array)
	{
		for (size_t i = 0; i < field->count; i++)
		{
			printf("  Value [%d]: %u\n", i+1, value[i]);
		}
	}
	else
	{
		for (size_t i = 0; i < field->count; i++)
		{
			if (field->size == 1)
			{
				if (field->usage_page == 0x09) // Mouse buttons
					printf("  Button %d, Value: %u\n", i+1, (value[0] >> i) & 0x1);
				else if (field->usage_page == 0x0c) // Customer page
					printf("  [0x%04x] %s, Value: %u\n", field->usage_ids[i + 1], get_usage_name(field->usage_ids[i + 1]), (value[0] >> i) & 0x1);
				else
					printf("  Bit %d, Value: %u\n", i, (value[0] >> i) & 0x1);
			}
			else
				printf("  Value [%d]: %u\n", i+1, value[i]);
		}
	}

	if (field->logic_range.min != INT16_MIN || field->logic_range.max != INT16_MAX)
	{
		printf("  Logic Range: %li to %li\n", field->logic_range.min, field->logic_range.max);
	}
	if (field->usage_range.min || field->usage_range.max)
	{
		printf("  Usage Range: 0x%02lx to 0x%02lx\n", field->usage_range.min, field->usage_range.max);
	}

	if (!field->flags.data)
		printf("  Flags: data\n");
	if (!field->flags.array)
		printf("  Flags: array\n");
	if (field->flags.relative)
		printf("  Flags: relative\n");

	printf("\n");
}

void app_main()
{
	// general usage callback
	hid_parser_set_callback(my_test_callback);

	// Set mouse-specific callback (if needed)
	hid_parser_set_mouse_callback(mouse_event_handler);

	// Test mouse descriptor
	printf("=== Mouse Report Descriptor ===\n");
	HidReportDescriptor *desc_mouse = hid_parser_init(hid_report_descriptor_mouse, sizeof(hid_report_descriptor_mouse));
	if (desc_mouse)
	{
		printf("Parsed %zu fields\n", desc_mouse->num_fields);
		uint8_t buf[] = {1, 0x7, 30, 40, 50};
		hid_parser_interpret_report(desc_mouse, buf, 5);
		hid_parser_free(desc_mouse);
	}
	else
	{
		printf("Failed to parse mouse descriptor.\n");
	}

	// Test keyboard descriptor
	printf("\n=== Keyboard Report Descriptor ===\n");
	HidReportDescriptor *desc_keyboard = hid_parser_init(hid_report_descriptor_keyboard, sizeof(hid_report_descriptor_keyboard));
	if (desc_keyboard)
	{
		printf("Parsed %zu fields\n", desc_keyboard->num_fields);
		uint8_t buf[] = {4, 7, 0, 4, 5, 6, 7, 8, 9};
		hid_parser_interpret_report(desc_keyboard, buf, 9);
		uint8_t buf1[] = {3, 7};
		hid_parser_interpret_report(desc_keyboard, buf1, 2);

		hid_parser_free(desc_keyboard);
	}
	else
	{
		printf("Failed to parse keyboard descriptor.\n");
	}
}
