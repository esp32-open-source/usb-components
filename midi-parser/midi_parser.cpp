#include "esp_err.h"
#include "esp_log.h"

#include "midi_parser.h"

#define TAG ""


MidiParser::MidiParser(HandlingMode mode) noexcept 
    : m_mode(mode), m_queue(nullptr), m_callback(nullptr),
      m_current_status(0), m_expected_data(0), m_sysex_active(false) {}

MidiParser::~MidiParser() {
    if (m_queue != nullptr) {
        vQueueDelete(m_queue);
        m_queue = nullptr;
    }
}

esp_err_t MidiParser::init(void (*callback)(const MidiMessage*)) {
    if (m_mode == HandlingMode::CALLBACK) {
        if (callback == nullptr) {
            return ESP_ERR_INVALID_ARG;
        }
        m_callback = callback;
        return ESP_OK;
    } else if (m_mode == HandlingMode::EVENT_QUEUE) {
        // Create a queue with default size 10; could be configurable via Kconfig
        m_queue = xQueueCreate(10, sizeof(MidiMessage*));
        if (m_queue == nullptr) {
            return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }
    return ESP_ERR_INVALID_STATE; // Shouldn't reach here with proper enum checks
}

void MidiParser::feed_raw_data(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        parse_byte(data[i]);
    }
}

esp_err_t MidiParser::get_next_message(MidiMessage& msg, TickType_t ticks_to_wait) {
    if (m_mode != HandlingMode::EVENT_QUEUE) {
        return ESP_ERR_INVALID_STATE;
    }

    MidiMessage* msg_ptr = nullptr;
    BaseType_t received = xQueueReceive(m_queue, &msg_ptr, ticks_to_wait);
    if (received != pdPASS) {
        return (received == pdFALSE) ? ESP_ERR_TIMEOUT : ESP_FAIL; // Adjust based on FreeRTOS queue errors
    }

    if (msg_ptr == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    msg = *msg_ptr; // Copy the message data to the output parameter
    delete[] msg_ptr; // Release allocated memory

    return ESP_OK;
}

void MidiParser::parse_byte(uint8_t byte) {
    if (m_sysex_active) {
        handle_sysex(byte);
    } else {
        handle_non_sysex(byte);
    }
}

void MidiParser::handle_sysex(uint8_t byte) {
    m_current_data.push_back(byte);

    // Check for Sysex end (0xF7)
    if (byte == 0xF7) {
        finalize_message();
        m_sysex_active = false;
    }
}

void MidiParser::handle_non_sysex(uint8_t byte) {
    // Check if the incoming byte is a status byte (MSB set)
    if ((byte & 0x80) != 0x00) { 
        // New status: finalize current message and start new
        finalize_message();
        start_new_status(byte);
    } else {
        // Data byte for current message
        m_current_data.push_back(byte);
        check_data_complete();
    }
}

void MidiParser::start_new_status(uint8_t status) {
    m_current_status = status;
    m_expected_data = expected_data_count(status);
    m_current_data.clear();

    // If the message has no data bytes, finalize immediately
    if (m_expected_data == 0) {
        finalize_message();
    }
}

size_t MidiParser::expected_data_count(uint8_t status) noexcept {
    uint8_t general = status & 0xF0;

    switch(general) {
        case 0x80: // Note Off
        case 0x90: // Note On
        case 0xA0: // Polyphonic Pressure (rare)
            return 2;
        case 0xB0: // Control Change
            return 2;
        case 0xC0: // Program Change
            return 1;
        case 0xD0: // Channel Pressure
            return 2;
        case 0xE0: // Pitch Bend
            return 2;

        // System Common messages (status codes 0xFx)
        default:
            switch(status) {
                case 0xF1: // MTC Quarter Frame
                    return 1;
                case 0xF2: // Song Select
                    return 1;
                case 0xF5: // Tune Request
                    return 1;
                case 0xF7: // Sysex (handled separately)
                    return 0; // Shouldn't reach here as Sysex is handled in state
                default:
                    return 0;
            }
    }
}

void MidiParser::check_data_complete() {
    if (m_current_data.size() == m_expected_data) {
        finalize_message();
    }
}

void MidiParser::finalize_message() {
    // Check if current status is Sysex start (0xF0)
    if (m_current_status == 0xF0 && !m_sysex_active) {
        // Start of Sysex message: add the 0xF0 to data and mark active
        m_current_data.insert(m_current_data.begin(), 0xF0);
        m_sysex_active = true; // Typo fixed from earlier (syex -> sysex)
        return;
    }

    MidiMessage msg;
    msg.status = m_current_status;
    msg.channel = m_current_status & 0x0F;

    // Truncate data to max 64 bytes
    size_t actual_data_len = std::min<size_t>(m_current_data.size(), sizeof(msg.data));
    msg.data_len = actual_data_len;
    std::memcpy(msg.data, m_current_data.data(), actual_data_len);

    // Check if Sysex message (ends with 0xF7)
    // Note: Sysex may be split, but this parser assumes contiguous data
    msg.is_sysex = false; // Will be set later if needed

    // Determine message type based on status byte
    uint8_t general_status = m_current_status & 0xF0;
    switch(general_status) {
        case 0x80:
            msg.type = MidiMessageType::NOTE_OFF;
            break;
        case 0x90:
            msg.type = MidiMessageType::NOTE_ON;
            break;
        case 0xA0:
            msg.type = MidiMessageType::KEY_PRESSURE;
            break;
        case 0xB0:
            msg.type = MidiMessageType::CONTROL_CHANGE;
            break;
        case 0xC0:
            msg.type = MidiMessageType::PROGRAM_CHANGE;
            break;
        case 0xD0:
            msg.type = MidiMessageType::CHANNEL_PRESSURE;
            break;
        case 0xE0:
            msg.type = MidiMessageType::PITCH_BEND;
            break;

        // System Common messages
        case 0xF0:
            msg.type = MidiMessageType::SYSTEM_EXCLUSIVE;
            msg.is_sysex = true;
            break;
        case 0xF1:
            msg.type = MidiMessageType::MTC_QUARTER_FRAME;
            break;
        case 0xF2:
            msg.type = MidiMessageType::SONG_SELECT;
            break;
        case 0xF3:
            msg.type = MidiMessageType::START; // Unison On (rare)
            break;
        case 0xF4:
            msg.type = MidiMessageType::CONTINUE; // Unison Off (rare)
            break;
        case 0xF5:
            msg.type = MidiMessageType::TUNE_REQUEST;
            break;
        case 0xF6: // Reserved
            msg.type = MidiMessageType::UNKNOWN;
            break;
        case 0xF7:
            msg.type = MidiMessageType::SYSTEM_EXCLUSIVE;
            msg.is_sysex = true;
            break;
        case 0xF8:
            msg.type = MidiMessageType::TIMING_CLOCK;
            break;
        case 0xF9: // Start (same as 0xF3, but standard)
            msg.type = MidiMessageType::START;
            break;
        case 0xFA: // Continue (same as 0xF4)
            msg.type = MidiMessageType::CONTINUE;
            break;
        case 0xFB:
            msg.type = MidiMessageType::STOP;
            break;
        case 0xFC:
            msg.type = MidiMessageType::ACTIVE_SENSING;
            break;
        case 0xFD:
            msg.type = MidiMessageType::RESET;
            break;
        default:
            msg.type = MidiMessageType::UNKNOWN;
    }

    // Special handling for Sysex (if not already set)
    if (msg.data_len > 0 && msg.data[0] == 0xF0) {
        msg.is_sysex = true;
        msg.type = MidiMessageType::SYSTEM_EXCLUSIVE;
    }

    deliver_message(&msg);

    // Reset state after finalizing
    m_current_status = 0;
    m_expected_data = 0;
    m_current_data.clear();
}

void MidiParser::deliver_message(const MidiMessage* msg) {
    if (m_mode == HandlingMode::CALLBACK) {
        m_callback(msg);
    } else if (m_mode == HandlingMode::EVENT_QUEUE) {
        // Allocate a copy of the message to store in the queue
        auto* copied_msg = new MidiMessage(*msg);
        if (copied_msg == nullptr) {
            ESP_LOGE("MidiParser", "Memory allocation failed for event queue");
            return;
        }

        // Send to queue; wait indefinitely (adjust if needed)
        BaseType_t success = xQueueSend(m_queue, copied_msg, 0);
        if (success != pdPASS) {
            ESP_LOGW("MidiParser", "Failed to send message to queue");
            delete copied_msg;
        }
    }
}

