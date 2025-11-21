
#include "../phase_shifter/phase_shifter.h"
#include <stdint.h>

#define STATUS_OK 0x00
#define STATUS_ERROR 0xFF

#define TEXT_MODE_TOKEN "TEXT_MODE"
#define PACKET_MODE_TOKEN "PACKET_MODE"
#define HELP_TOKEN "HELP"

#define TOKEN_SEPARATOR " "

#define COMMAND_SET_PHASE_SHIFT 0x01
#define SET_PHASE_SHIFT_COMMAND_TOKEN "SET_PHASE_SHIFT"
typedef struct {
    uint8_t phase_shifter_values[PHASE_SHIFTER_COUNT];
} set_phase_shift_command_t;

#define COMMAND_PHASE_SHIFTER_LATCH 0x02
#define PHASE_SHIFTER_LATCH_COMMAND_TOKEN "PHASE_SHIFTER_LATCH"
typedef struct {
    uint8_t latch_state : 1;
} phase_shifter_latch_command_t;

#define COMMAND_CONFIG 0x03
#define CONFIG_COMMAND_TOKEN "CONFIG"
typedef struct {
    uint8_t auto_latch : 1;
} config_command_t;

#define COMMAND_PING 0xFE
#define PING_COMMAND_TOKEN "PING"
typedef struct {
    uint8_t ping_data;
} ping_command_t;

#define COMMAND_RESPONSE 0xFF
#define COMMAND_RESPONSE_TOKEN "COMMAND_RESPONSE"
typedef struct {
    uint8_t status_code;
    uint8_t original_command;
} command_response_t;

const char* command_tokens[] = {
    SET_PHASE_SHIFT_COMMAND_TOKEN,
    PHASE_SHIFTER_LATCH_COMMAND_TOKEN,
    CONFIG_COMMAND_TOKEN,
    PING_COMMAND_TOKEN,
    COMMAND_RESPONSE_TOKEN
};
const uint8_t command_ids[] = {
    COMMAND_SET_PHASE_SHIFT,
    COMMAND_PHASE_SHIFTER_LATCH,
    COMMAND_CONFIG,
    COMMAND_PING,
    COMMAND_RESPONSE
};
#define COMMAND_TOKEN_COUNT (sizeof(command_tokens) / sizeof(command_tokens[0]))
