# MIDI-Parser Component (AI-Generated)

## Overview
This AI-generated MIDI parser component processes MIDI 1.0 and 2.0 files/streams, supporting both callback-driven event handling and polling-based message retrieval. Designed for real-time audio applications and music software integration.

---

## Key Features
### 1. MIDI Spec Compliance
- **MIDI 1.0**: Full support for Standard MIDI Files (SMF) v1/v2, including track metadata, tempo changes, and program/clock messages.
- **MIDI 2.0**: Handles Extended MIDI (XMI) formats, including Bulk Dump Protocol (BDP), SysEx/Sysexi messages, and Meta Events with 3-byte status codes.

### 2. Event Handling Modes
- **Callback Mode**: Register custom event handlers for note_on/off, control_change, program_change, etc. Triggers immediately as events are parsed.
- **Polling Mode**: Collects events in a buffer; retrieve them via `get_events()` for batch processing or synchronous access.

### 3. Performance Optimizations
- Stream-based parsing (no full file load required) for low-latency audio applications.
- Efficient memory management with dynamic buffer resizing and zero-copy event extraction where possible.


### Note

Component is in usb related repository, but it is peripheral independent, so can be used with BLE midi too.

