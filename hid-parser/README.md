## 🔧 **Overview**

This is a C-based **HID (Human Interface Device) Report Descriptor parser** that:

- Parses USB HID reports and extracts structured information.
- Interprets raw report data using the parsed descriptor.
- Provides callback mechanisms for event handling.

It supports various usage types including **mouse**, **keyboard**, **joystick**, and more, with a flexible and extensible design.

---

## 📦 **Key Features**

### 1. **Descriptor Parsing**
- Parses standard USB HID Report Descriptors.
- Extracts field metadata such as:
  - Usage Page
  - Usage ID (from `0x01` Generic Desktop and other pages)
  - Size in bits (1 to 32)
  - Offset within the report
  - Count of items
  - Flags (e.g., relative, absolute)

### 2. **Report Interpretation**
- Extracts values from bit fields.
- Supports different data sizes: 1-bit, 8-bit, 16-bit, and 32-bit.
- Handles signed and unsigned values.

### 3. **Callback Support**
- Allows custom event handling for every field in the report:
  - Generic callback
  - Mouse-specific callback (for X/Y axis, wheel)

---

## 🧩 **Usage Example**

```c
#include "hid_parser.h"

void mouse_callback(const HidFieldInfo *field, uint8_t buttons,
                    int16_t xAxis, int16_t yAxis, int16_t wheel) {
    // Handle mouse movement or button presses
}
void my_test_callback(const HidFieldInfo *field, const uint8_t *value, size_t len, uint8_t report_id)
{
}
void app_main()
{
    const uint8_t hid_desc[] = { /* HID descriptor data */ };
    const uint8_t report_data[] = { /* Raw report data */ };

    HidReportDescriptor *desc = hid_parser_init(hid_desc, sizeof(hid_desc));
    if (!desc) return -1;

	// general usage callback
	hid_parser_set_callback(my_test_callback);
    // and/or for mouse scpecialized callback
    hid_parser_set_mouse_callback(mouse_callback);

    hid_parser_interpret_report(desc, report_data, sizeof(report_data));

    hid_parser_free(desc);
}
```

---

## 📦 **Data Structures**

### `HidFieldInfo`
- Stores metadata for each HID field:
  - Usage page and ID
  - Offset within the report (in bits)
  - Size in bits
  - Count of items per field
  - Flags indicating behavior: relative, absolute, etc.
  - Logical range (min/max)

### `HidReportDescriptor`
- Container for all parsed fields.

---

## 📌 **Supported Usage IDs**

| Usage ID | Description |
|----------|-------------|
| `USAGE_X_AXIS` | X-axis movement |
| `USAGE_Y_AXIS` | Y-axis movement |
| `USAGE_WHEEL` | Mouse wheel |
| `USAGE_KEYBOARD_LEFT_CONTROL` | Left Control key |
| `USAGE_KEYBOARD_LEFT_SHIFT` | Left Shift key |
| `USAGE_KEYBOARD_LEFT_ALT` | Left Alt key |
| `USAGE_KEYBOARD_LEFT_GUI` | Left GUI (Windows) key |

> Additional usage IDs from the **Consumer Page** are also supported.

---

## 📦 **Callback Types**

### 1. General Callback
```c
typedef void (*HidEventCallback)(const HidFieldInfo *field,
                                 const uint8_t *value, size_t len, uint8_t report_id);
```

### 2. Mouse-Specific Callback
```c
typedef void (*HidMouseCallback)(const HidFieldInfo *field,
                                 uint8_t buttons, int16_t xAxis, int16_t yAxis, int16_t wheel);
```

---

## 🧩 **Extensibility**

- Easy to add support for:
  - More HID usage pages.
  - Gamepad or joystick input handling.
  - Multi-byte usages and vendor-specific reports.

---


---

# 📝 **TODO List - HID Parser Library**

## ✅ **Pending Features & Improvements**

---

### 🔧 1. **Add Keyboard Callback (Currently Missing)**
- Implement a dedicated keyboard callback that handles:
  - Key press/release events.
  - Modifier key state (Control, Shift, Alt, GUI).
  - Keyboards with multiple layouts or layers.

**Status:** ⏳ In Progress

---

### 🧩 2. **Add Full Support for Consumer Page (Usage Page 0x0C)**
- Currently only partial support exists.
- Need to:
  - Parse and interpret usage IDs like `USAGE_CONSUMER_VOLUME`, `USAGE_CONSUMER_MUTE`, etc.
  - Provide meaningful callbacks or event handling.

**Status:** ⏳ In Progress

---

### 🎮 3. **Add Support for Game Controllers (Joysticks, Triggers, Buttons)**
- Extend the parser to handle:
  - Axes: X/Y/Z, RX/RY/RZ
  - Buttons: up to 32 or more
  - Triggers and D-pads
- Add dedicated callbacks or event types like `GAMEPAD_AXIS`, `GAMEPAD_BUTTON`.

**Status:** ⏳ In Progress

---

### 🧹 4. **Code Cleanup**
- Remove unused functions, variables, or code fragments.
- Clean up comments (if any remain).
- Fix formatting inconsistencies.

**Status:** ✅ Done (Mostly)

---

### 📚 5. **Update Docstrings and Documentation**
- Ensure all public API functions have complete and accurate documentation.
- Improve internal comments for clarity.
- Update README with usage examples, supported devices, and feature status.

**Status:** ⏳ In Progress

---

## 📈 **Future Roadmap**

| Feature | Priority | Status |
|--------|----------|--------|
| Keyboard Callback | High | ⏳ In Progress |
| Consumer Page Support | Medium | ⏳ In Progress |
| Game Controller Support | Medium | ⏳ In Progress |
| API Documentation Update | Low | ⏳ In Progress |
| Code Cleanup | Low | ✅ Done |

---

## 📌 **How to Contribute**

- Submit pull requests for any of the above features.
- Report bugs or suggest improvements in the issue tracker.

---

## 🔐 **License**
MIT License — see `LICENSE` file.

