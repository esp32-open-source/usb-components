/**
 * @file midi_parser.h 
 * @author Dariusz Krempa  (esp32@esp32.eu.org)
 * @brief 
 * @version 0.1
 * @date 2025-05-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef USB_MIDI_PARSER_H_
#define USB_MIDI_PARSER_H_

#include <cstdint>
#include <cstring>
#include <vector>

// FreeRTOS includes for queue handling
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Type of MIDI message.
 */
enum class MidiMessageType {
    NOTE_ON,
    NOTE_OFF,
    KEY_PRESSURE,  // Polyphonic Aftertouch
    CONTROL_CHANGE,
    PROGRAM_CHANGE,
    CHANNEL_PRESSURE,  // Channel Aftertouch
    PITCH_BEND,
    TIMING_CLOCK,
    START,
    CONTINUE,
    STOP,
    ACTIVE_SENSING,
    RESET,
    MTC_QUARTER_FRAME,  // MIDI Time Code Quarter Frame
    SONG_SELECT,
    TUNE_REQUEST,
    SYSTEM_EXCLUSIVE,   // System Exclusive (Sysex)
    UNKNOWN             // Unrecognized status byte
};

/**
 * @brief A parsed MIDI message structure.
 */
struct MidiMessage {
    MidiMessageType type;       ///< Message type
    uint8_t status;              ///< Raw status byte (0x80-0xFF)
    bool is_sysex : 1;           ///< True if Sysex message
    size_t channel : 4;          ///< Channel number (0-15, valid for voice messages)
    size_t data_len : 6;         ///< Number of data bytes (max 63)
    uint8_t data[64];            ///< Data bytes (truncated to 64 bytes)

    MidiMessage() noexcept 
        : type(MidiMessageType::UNKNOWN), status(0), is_sysex(false), channel(0), data_len(0) {
        std::memset(data, 0, sizeof(data));
    }
};

/**
 * @brief Delivery mode for parsed messages.
 */
enum class HandlingMode { CALLBACK, EVENT_QUEUE };

/**
 * @brief USB MIDI Parser class.
 * Parses raw USB MIDI byte streams into structured MidiMessage objects and delivers via callback or event queue.
 */
class MidiParser {
public:
    /**
     * @brief Constructor.
     * @param mode Delivery mode (CALLBACK or EVENT_QUEUE).
     */
    explicit MidiParser(HandlingMode mode) noexcept;

    /**
     * @brief Destructor.
     */
    ~MidiParser();

    /**
     * @brief Initialize the parser with a callback (required for CALLBACK mode).
     * @param callback Callback function to invoke on message arrival (CALLBACK mode only).
     * @return ESP_OK on success, error code otherwise.
     */
    esp_err_t init(void (*callback)(const MidiMessage*) = nullptr);

    /**
     * @brief Feed raw USB MIDI data to the parser for processing.
     * @param data Raw byte stream from USB.
     * @param len Length of data in bytes.
     */
    void feed_raw_data(const uint8_t* data, size_t len);

    /**
     * @brief Retrieve next parsed message from event queue (EVENT_QUEUE mode only).
     * @param[out] msg Populated with the received message.
     * @param ticks_to_wait Timeout in FreeRTOS ticks (portMAX_DELAY for no timeout).
     * @return ESP_OK if successful, error code otherwise.
     */
    esp_err_t get_next_message(MidiMessage& msg, TickType_t ticks_to_wait = portMAX_DELAY);

private:
    HandlingMode m_mode;               ///< Message delivery mode
    QueueHandle_t m_queue;             ///< Event queue (EVENT_QUEUE mode)
    void (*m_callback)(const MidiMessage*); ///< Callback function (CALLBACK mode)

    // Parsing state variables
    uint8_t m_current_status = 0;      ///< Current message status byte
    size_t m_expected_data = 0;        ///< Expected remaining data bytes for current message
    bool m_sysex_active = false;       ///< True if parsing a Sysex message (0xF0...0xF7)
    std::vector<uint8_t> m_current_data; ///< Accumulated data bytes for current message

    /**
     * @brief Process a single byte of incoming USB MIDI data.
     * @param byte Incoming byte to process.
     */
    void parse_byte(uint8_t byte);

    /**
     * @brief Handle a byte during Sysex message parsing.
     * @param byte Byte received while in Sysex mode.
     */
    void handle_sysex(uint8_t byte);

    /**
     * @brief Handle a byte outside of Sysex mode (standard MIDI messages).
     * @param byte Incoming byte.
     */
    void handle_non_sysex(uint8_t byte);

    /**
     * @brief Initialize parsing of a new status byte.
     * @param status New status byte received.
     */
    void start_new_status(uint8_t status);

    /**
     * @brief Determine the number of data bytes expected for a given status byte.
     * @param status Status byte (0x80-0xFF).
     * @return Number of required data bytes (0, 1, or 2).
     */
    static size_t expected_data_count(uint8_t status) noexcept;

    /**
     * @brief Check if current message has received all expected data bytes.
     */
    void check_data_complete();

    /**
     * @brief Finalize the current message, populate MidiMessage, and deliver it.
     */
    void finalize_message();

    /**
     * @brief Deliver a parsed message to either callback or event queue.
     * @param msg Pointer to the parsed message.
     */
    void deliver_message(const MidiMessage* msg);
};

#endif /* USB_MIDI_PARSER_H_ */
