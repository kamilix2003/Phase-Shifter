#pragma once

#include "config.h"

#define CMD_PING 0x00
typedef struct {
    uint8_t payload[8];
} cmd_ping_t;
void cmd_ping_handler(cmd_ping_t* cmd);

#define CMD_PONG 0x01
typedef struct {
    uint8_t payload[8];
} cmd_pong_t;
void cmd_pong_handler(cmd_pong_t* cmd);

#define CMD_SET_PHASE_SHIFT 0x10
typedef struct {
    uint8_t phase_shift[PHASE_SHIFTER_COUNT];
} cmd_set_phase_shift_t;
void cmd_set_phase_shift_handler(cmd_set_phase_shift_t* cmd);

#define CMD_TRANSMIT_PHASE_SHIFT 0x11
typedef struct {
    // No additional context needed for transmit command
} cmd_transmit_phase_shift_t;
void cmd_transmit_phase_shift_handler(cmd_transmit_phase_shift_t* cmd);

#define CMD_SET_LATCH_STATE 0x12
typedef struct {
    uint8_t latch_state;
} cmd_set_latch_state_t;
void cmd_set_latch_state_handler(cmd_set_latch_state_t* cmd);