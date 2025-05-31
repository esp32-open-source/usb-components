#include "hid_parser.h"
#include <string.h>
#include <stdlib.h>

// Global callback variables
static HidEventCallback g_callback = NULL;
static HidMouseCallback g_mouse_callback = NULL;
static HidEventCallback g_keyboard_callback = NULL;

// Set callback function
void hid_parser_set_callback(HidEventCallback callback)
{
	g_callback = callback;
}

// Set the callback based on usage page
void hid_parser_set_mouse_callback(HidMouseCallback callback)
{
	g_mouse_callback = callback;
}

void hid_parser_set_keyboard_callback(HidEventCallback callback)
{
	g_keyboard_callback = callback;
}

#define MAX_COLLECTION_STACK 16

uint32_t parse_unsigned_hid_value(const uint8_t *data, uint8_t size)
{
	uint32_t value = 0;
	for (uint8_t i = 0; i < size && i < 4; i++)
	{
		value |= ((uint32_t)data[i]) << (8 * i);
	}
	return value;
}

static int32_t parse_signed_hid_value(const uint8_t *data, uint8_t size)
{
	uint32_t raw = 0;
	for (uint8_t i = 0; i < size && i < 4; i++)
	{
		raw |= ((uint32_t)data[i]) << (8 * i);
	}
	switch (size)
	{
	case 1:
		return (int32_t)(int8_t)raw;
	case 2:
		return (int32_t)(int16_t)raw;
	case 4:
		return (int32_t)raw;
	default:
		return (int32_t)raw;
	}
}

HidReportDescriptor *hid_parser_init(const uint8_t *desc, size_t desc_size)
{
	HidReportDescriptor *desc_info = (HidReportDescriptor *)malloc(sizeof(HidReportDescriptor));
	if (!desc_info)
		return NULL;
	desc_info->num_fields = 0;
	desc_info->fields = NULL;

	uint8_t current_report_id = 0;
	int field_count = 0;

	typedef struct
	{
		uint8_t type; // 0x01 = Application, 0x00 = Physical
		uint8_t page;
		uint16_t usage_page;
		uint16_t usage_id[10];
		int8_t usage_count;
		struct
		{
			int32_t min;
			int32_t max;
		} logic_range;

		struct
		{
			uint32_t min;
			uint32_t max;
		} usage_range;
	} CollectionContext;

	CollectionContext collection_stack[MAX_COLLECTION_STACK] = {};
	int stack_top = 0;
	collection_stack[stack_top].usage_count = 0;
	uint32_t field_offset = 0; // Start from 0

	typedef struct
	{
		uint8_t report_id; // Current Report ID in effect
	} ParseContext;

	HidFieldInfo tmp_field;
	memset(&tmp_field, 0, sizeof(HidFieldInfo));
	ParseContext context;
	context.report_id = 0;

	for (size_t i = 0; i < desc_size;)
	{
		uint8_t b = desc[i++];
		if (b == 0x05)
		{
			if (i >= desc_size)
				break;
			uint8_t usage_page = desc[i++];
			collection_stack[stack_top].usage_page = usage_page;
			tmp_field.usage_page = usage_page;
		}
		else if (b == 0x06)
		{
			if (i + 1 >= desc_size)
				break;
			uint8_t byte1 = desc[i++], byte2 = desc[i++];
			uint16_t usage_page = (byte2 << 8) | byte1;
			collection_stack[stack_top].usage_page = usage_page;
			tmp_field.usage_page = usage_page;
		}
		else if (b == 0x09 || b == 0x0A)
		{
			uint16_t usage_id = 0;
			if (b == 0x09)
			{
				if (i >= desc_size)
					break;
				uint8_t usage = desc[i++];
				usage_id = usage;
			}
			else
			{
				if (i + 1 >= desc_size)
					break;
				uint8_t byte1 = desc[i++], byte2 = desc[i++];
				usage_id = (byte2 << 8) | byte1;
			}
			tmp_field.usage_ids[collection_stack[stack_top].usage_count++] = usage_id;
			tmp_field.usage_id = usage_id;
		}
		else if (b == 0x15 || b == 0x16 || b == 0x17 || b == 0x25 || b == 0x26 || b == 0x27)
		{
			uint8_t size = b & 0x03;
			if (i + size > desc_size)
				break;
			uint32_t value = parse_signed_hid_value(&desc[i], size);
			i += size;
			if ((b & 0xF0) == 0x10)
				collection_stack[stack_top].logic_range.min = value;
			else
				collection_stack[stack_top].logic_range.max = value;
		}
		else if (b == 0x19 || b == 0x1A || b == 0x1B || b == 0x29 || b == 0x2A || b == 0x2B)
		{
			uint8_t size = b & 0x03;
			if (i + size > desc_size)
				break;
			uint32_t value = parse_unsigned_hid_value(&desc[i], size);
			i += size;
			if ((b & 0xF0) == 0x10)
				collection_stack[stack_top].usage_range.min = value;
			else
				collection_stack[stack_top].usage_range.max = value;
		}
		else if (b == 0x75)
		{
			if (i >= desc_size)
				break;
			uint8_t size = desc[i++];
			tmp_field.size = size;
		}
		else if (b == 0x95)
		{
			if (i >= desc_size)
				break;
			uint8_t count = desc[i++];
			tmp_field.count = count;
		}
		else if (b == 0x81 || b == 0x91 || b == 0xB1)
		{
			if (i >= desc_size)
				break;
			uint8_t type = desc[i++];
			HidFieldInfo *new_fields = realloc(desc_info->fields, (field_count + 1) * sizeof(HidFieldInfo));
			if (!new_fields)
			{
				free(desc_info);
				return NULL;
			}
			desc_info->fields = new_fields;
			tmp_field.flags.val = type;
			tmp_field.report_id = context.report_id;
			tmp_field.offset = field_offset + (context.report_id ? 0 : 0);
			tmp_field.usage_page = collection_stack[stack_top].usage_page;
			tmp_field.logic_range.min = collection_stack[stack_top].logic_range.min;
			tmp_field.logic_range.max = collection_stack[stack_top].logic_range.max;
			tmp_field.usage_range.min = collection_stack[stack_top].usage_range.min;
			tmp_field.usage_range.max = collection_stack[stack_top].usage_range.max;
			memcpy(&desc_info->fields[field_count++], &tmp_field, sizeof(HidFieldInfo));
			field_offset += tmp_field.size * tmp_field.count;
			collection_stack[stack_top].usage_count = 0;
		}
		else if (b == 0x85)
		{
			context.report_id = desc[i++];
			field_offset = 0;
		}
	}

	desc_info->num_fields = field_count;
	return desc_info;
}

void hid_parser_free(HidReportDescriptor *desc)
{
	if (!desc)
		return;
	free(desc->fields);
	free(desc);
}

// Interpret the report and call the user callback with full HidFieldInfo context
void hid_parser_interpret_report(const HidReportDescriptor *desc, const uint8_t *report_data, size_t report_size)
{
	for (size_t i = 0; i < desc->num_fields; i++)
	{
		const HidFieldInfo *field = &desc->fields[i];
		if ((field->flags.val & 0x03) == 0x03)
			continue;

		uint8_t byte_index = field->offset / 8;
		uint8_t bit_offset = field->offset % 8;
		if (field->report_id)
			byte_index++;

		if (byte_index >= report_size || (field->report_id && report_data[0] != field->report_id))
			continue;

		int32_t value = 0;
		const uint8_t *val_ptr = &report_data[byte_index];
		size_t len = 1;

		if (g_callback)
			g_callback(field, val_ptr, len, field->report_id);

		if (field->usage_page == 0x01)
		{
			uint8_t n = 0;
			if (field->report_id)
			{
				n++;
			}

			if ((field->usage_id == USAGE_X_AXIS || field->usage_id == USAGE_Y_AXIS || field->usage_id == USAGE_WHEEL))
			{
				int16_t x = 0, y = 0, w = 0;
				uint8_t b = report_data[n];
				if (g_mouse_callback)
				{
					if (field->size == 8)
					{
						x = report_data[n + 1];
						y = report_data[n + 2];
						w = report_data[n + 3];
					}
					else if (field->size == 16)
					{
						x = report_data[n + 1] << 8 | report_data[n + 2];
						y = report_data[n + 3] << 8 | report_data[n + 4];
						w = report_data[n + 5] << 8 | report_data[n + 6];
					}
				}
				g_mouse_callback(field, b, x, y, w);
			}
			// TODO
			else if (field->usage_id == USAGE_KEYBOARD_LEFT_CONTROL || field->usage_id == USAGE_KEYBOARD_LEFT_SHIFT)
			{
				if (g_keyboard_callback)
					g_keyboard_callback(field, val_ptr, len, field->report_id);
			}
		}
	}
}
