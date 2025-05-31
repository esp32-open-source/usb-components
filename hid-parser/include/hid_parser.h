#ifndef HID_PARSER_H
#define HID_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "keycodes.h"

/**
 * @file hid_parser.h
 * @brief Header file for HID report descriptor parser with usage ID definitions and data structures
 */

/**
 * @defgroup UsageIDs HID Usage IDs
 * @brief Common HID Usage Page (0x01) and Consumer Page (0x0C) Usage IDs
 * @{
 */

// General Desktop Controls (Usage Page 0x01)
#define USAGE_POINTER 0x0101 ///< Pointer device
#define USAGE_MOUSE 0x0102   ///< Mouse
#define USAGE_JOYSTICK 0x0103 ///< Joystick
#define USAGE_GAMEPAD 0x0104 ///< Gamepad
#define USAGE_KEYBOARD 0x0105 ///< Keyboard
#define USAGE_KEYPAD 0x0106  ///< Keypad
#define USAGE_BUTTON 0x0109  ///< Button

// Axes (Usage Page 0x01)
#define USAGE_X_AXIS 0x30 ///< X-axis
#define USAGE_Y_AXIS 0x31 ///< Y-axis
#define USAGE_Z_AXIS 0x32 ///< Z-axis
#define USAGE_RX_AXIS 0x33 ///< Rotary X-axis
#define USAGE_RY_AXIS 0x34 ///< Rotary Y-axis
#define USAGE_RZ_AXIS 0x35 ///< Rotary Z-axis
#define USAGE_SLIDER 0x36  ///< Slider
#define USAGE_DIAL 0x37    ///< Dial
#define USAGE_WHEEL 0x38   ///< Wheel
#define USAGE_HAT_SWITCH 0x39 ///< Hat switch

// System Controls (Usage Page 0x01)
#define USAGE_COUNTERS 0x0140     ///< Counters
#define USAGE_TOGGLE_SWITCHES 0x0141 ///< Toggle switches
#define USAGE_LEDS 0x0142         ///< LEDs
#define USAGE_KEYBOARD_LEFT_CONTROL 0x0706 ///< Keyboard Left Control
#define USAGE_KEYBOARD_LEFT_SHIFT 0x0707   ///< Keyboard Left Shift
#define USAGE_KEYBOARD_LEFT_ALT 0x0708     ///< Keyboard Left Alt
#define USAGE_KEYBOARD_LEFT_GUI 0x0709     ///< Keyboard Left GUI

// Consumer Page (Usage Page 0x0C)
#define USAGE_CONSUMER_CONTROL 0x0C00         ///< Consumer Control
#define USAGE_CONSUMER_POWER 0x0C30           ///< Power
#define USAGE_CONSUMER_RESET 0x0C31          ///< Reset
#define USAGE_CONSUMER_SLEEP 0x0C32          ///< Sleep
#define USAGE_CONSUMER_MENU 0x0C40           ///< Menu
#define USAGE_CONSUMER_SELECTION 0x0C41      ///< Selection
#define USAGE_CONSUMER_ASSIGN_SEL 0x0C42     ///< Assign Select
#define USAGE_CONSUMER_MODE_STEP 0x0C43      ///< Mode Step
#define USAGE_CONSUMER_RECALL_LAST 0x0C83    ///< Recall Last
#define USAGE_CONSUMER_CHANNEL_UP 0x0C9C     ///< Channel Up
#define USAGE_CONSUMER_CHANNEL_DOWN 0x0C9D   ///< Channel Down
#define USAGE_CONSUMER_PLAY 0x0B00           ///< Play
#define USAGE_CONSUMER_PAUSE 0x0B01          ///< Pause
#define USAGE_CONSUMER_RECORD 0x0B02         ///< Record
#define USAGE_CONSUMER_FAST_FORWARD 0x0B03   ///< Fast Forward
#define USAGE_CONSUMER_REWIND 0x0B04         ///< Rewind
#define USAGE_CONSUMER_EJECT 0x0B08          ///< Eject
#define USAGE_CONSUMER_RANDOM_PLAY 0x0B0A    ///< Random Play
#define USAGE_CONSUMER_REPEAT 0x0B0B         ///< Repeat
#define USAGE_CONSUMER_VOLUME 0x0C80         ///< Volume
#define USAGE_CONSUMER_VOLUME_INCREMENT 0x0C90 ///< Volume Increment
#define USAGE_CONSUMER_VOLUME_DECREMENT 0x0C91 ///< Volume Decrement
#define USAGE_CONSUMER_MUTE 0x0C9F           ///< Mute

#define USAGE_CONSUMER_PLAY_PAUSE        0x00CD
#define USAGE_CONSUMER_SCAN_NEXT_TRACK   0x00B5
#define USAGE_CONSUMER_SCAN_PREV_TRACK   0x00B6
#define USAGE_CONSUMER_STOP              0x00B7
#define USAGE_CONSUMER_AC_HOME           0x0223
#define USAGE_CONSUMER_AC_BACK           0x0224
#define USAGE_CONSUMER_AC_FORWARD        0x0225
/** @} */

#define MOD_LEFT_CTRL   0x01  // Bit 0
#define MOD_LEFT_SHIFT  0x02  // Bit 1
#define MOD_LEFT_ALT    0x04  // Bit 2
#define MOD_LEFT_GUI    0x08  // Bit 3
#define MOD_RIGHT_CTRL  0x10  // Bit 4
#define MOD_RIGHT_SHIFT 0x20  // Bit 5
#define MOD_RIGHT_ALT   0x40  // Bit 6
#define MOD_RIGHT_GUI   0x80  // Bit 7

static const char *get_usage_name(uint16_t usage_id)
{
    switch (usage_id)
    {
    // Generic Desktop Page (0x01)
    case USAGE_POINTER: return "Pointer";
    case USAGE_MOUSE: return "Mouse";
    case USAGE_JOYSTICK: return "Joystick";
    case USAGE_GAMEPAD: return "Game Pad";
    case USAGE_KEYBOARD: return "Keyboard";
    case USAGE_KEYPAD: return "Keypad";
    case USAGE_X_AXIS: return "X-axis";
    case USAGE_Y_AXIS: return "Y-axis";
    case USAGE_Z_AXIS: return "Z-axis";
    case USAGE_RX_AXIS: return "Rx-axis";
    case USAGE_RY_AXIS: return "Ry-axis";
    case USAGE_RZ_AXIS: return "Rz-axis";
    case USAGE_SLIDER: return "Slider";
    case USAGE_DIAL: return "Dial";
    case USAGE_WHEEL: return "Wheel";
    case USAGE_HAT_SWITCH: return "Hat switch";
    case USAGE_COUNTERS: return "Counters";
    case USAGE_TOGGLE_SWITCHES: return "Toggle switches";
    case USAGE_LEDS: return "LEDs";
    case USAGE_KEYBOARD_LEFT_CONTROL: return "Keyboard Left Control";
    case USAGE_KEYBOARD_LEFT_SHIFT: return "Keyboard Left Shift";
    case USAGE_KEYBOARD_LEFT_ALT: return "Keyboard Left Alt";
    case USAGE_KEYBOARD_LEFT_GUI: return "Keyboard Left GUI";
    case USAGE_BUTTON: return "Button";

    // Consumer Page (0x0C)
    case USAGE_CONSUMER_CONTROL: return "Consumer Control";
    case USAGE_CONSUMER_POWER: return "Power";
    case USAGE_CONSUMER_RESET: return "Reset";
    case USAGE_CONSUMER_SLEEP: return "Sleep";
    case USAGE_CONSUMER_MENU: return "Menu";
    case USAGE_CONSUMER_SELECTION: return "Selection";
    case USAGE_CONSUMER_ASSIGN_SEL: return "Assign Selection";
    case USAGE_CONSUMER_MODE_STEP: return "Mode Step";
    case USAGE_CONSUMER_RECALL_LAST: return "Recall Last";
    case USAGE_CONSUMER_CHANNEL_UP: return "Channel Up";
    case USAGE_CONSUMER_CHANNEL_DOWN: return "Channel Down";
    case USAGE_CONSUMER_PLAY: return "Play";
    case USAGE_CONSUMER_PAUSE: return "Pause";
    case USAGE_CONSUMER_RECORD: return "Record";
    case USAGE_CONSUMER_FAST_FORWARD: return "Fast Forward";
    case USAGE_CONSUMER_REWIND: return "Rewind";
    case USAGE_CONSUMER_SCAN_NEXT_TRACK: return "Next Track";
    case USAGE_CONSUMER_SCAN_PREV_TRACK: return "Previous Track";
    case USAGE_CONSUMER_STOP: return "Stop";
    case USAGE_CONSUMER_EJECT: return "Eject";
    case USAGE_CONSUMER_RANDOM_PLAY: return "Random Play";
    case USAGE_CONSUMER_REPEAT: return "Repeat";
    case USAGE_CONSUMER_VOLUME: return "Volume";
    case USAGE_CONSUMER_VOLUME_INCREMENT: return "Volume +";
    case USAGE_CONSUMER_VOLUME_DECREMENT: return "Volume -";
    case USAGE_CONSUMER_MUTE: return "Mute";

    default:
        if ((usage_id & 0xFF00) == 0x0100)
        {
            return "Generic Desktop Usage";
        }
        else if ((usage_id & 0xFF00) == 0x0C00)
        {
            return "Consumer Usage";
        }
        return "Unknown Usage";
    }
}

/**
 * @defgroup ParserStructs Internal Data Structures
 * @{
 */

// Structure for tracking current parsing context
typedef struct {
    uint8_t report_id; // Current Report ID in effect
} ParseContext;

// Structure to hold parsed HID field information
typedef struct {
    const char *name;       ///< Field name (e.g., "button", "axis")
    uint8_t report_id;      ///< Report ID associated with this field
    uint8_t *data;          ///< Pointer to first byte of value in report data
    uint16_t offset;        ///< Bit offset in report (0-n)
    uint8_t size;           ///< Size in bits (1-32)
    uint8_t count;          ///< Number of items in an array (e.g., 5 buttons)
    uint16_t usage_page;    ///< Usage page ID
    uint16_t usage_ids[32]; ///< Store up to 32 usages per field
    uint16_t usage_id;      ///< Usage ID within the page
    const char *usage_name; ///< Human-readable name of the usage

    union {
        struct {
            uint8_t data : 1;       ///< Constant value (1) or variable (0)
            uint8_t array : 1;      ///< Array of values (1) or single value (0)
            uint8_t relative : 1;   ///< Relative value
            uint8_t wrap : 1;       ///< Wraps around (e.g., hat switch)
            uint8_t nonlinear : 1;  ///< Non-linear mapping
            uint8_t preferred : 1;  ///< Preferred state
            uint8_t null : 1;       ///< No null position
            uint8_t dummy : 1;      ///< Unused bit
        };
        uint8_t val;
    } flags;

    struct {
        int32_t min;
        int32_t max;
    } logic_range;

    struct {
        uint32_t min;
        uint32_t max;
    } usage_range;
} HidFieldInfo;

typedef struct {
    uint8_t report_id;
    const HidFieldInfo *field;
    int32_t values[32]; // Max of 32 usage IDs per field
    size_t count;       ///< Number of valid values in the array
} HidEvent;

// Structure to store the parsed HID report descriptor info
typedef struct
{
    size_t num_fields;
    HidFieldInfo *fields;
} HidReportDescriptor;
/** @} */

/**
 * @defgroup ParserFunctions Public API Functions
 * @{
 */

 // Forward declaration of the callback type
typedef void (*HidEventCallback)(const HidFieldInfo *field,
                                 const uint8_t *value, size_t len, uint8_t report_id);

typedef void (*HidMouseCallback)(const HidFieldInfo *field, uint8_t buttons, int16_t xAxis, int16_t yAxis, int16_t wheel);
// Initialize and parse the HID Report Descriptor
HidReportDescriptor *hid_parser_init(const uint8_t *desc, size_t desc_size);

// Free memory allocated by parser
void hid_parser_free(HidReportDescriptor *desc);

// Set callback function for event handling
void hid_parser_set_callback(HidEventCallback callback);
void hid_parser_set_mouse_callback(HidMouseCallback callback);
void hid_parser_set_keyboard_callback(HidEventCallback callback);

// Interpret raw report data and trigger callbacks
void hid_parser_interpret_report(const HidReportDescriptor *desc, const uint8_t *report_data, size_t report_size);

/** @} */

#endif // HID_PARSER_H
