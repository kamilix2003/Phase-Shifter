
#include "../phase_shifter/phase_shifter.h"
#include <stdint.h>

#define STATUS_OK 0x00
#define STATUS_ERROR 0xFF

#define COMMAND_SET_PHASE_SHIFT 0x01
typedef struct {
    uint8_t phase_shifter_values[PHASE_SHIFTER_COUNT];
} set_phase_shift_command_t;
#define EXPECTED_LENGTH_SET_PHASE_SHIFT PHASE_SHIFTER_COUNT

#define COMMAND_PHASE_SHIFTER_LATCH 0x02
typedef struct {
    uint8_t latch_state : 1;
} phase_shifter_latch_command_t;
#define EXPECTED_LENGTH_SET_PHASE_SHIFTER_LATCH 1

#define COMMAND_CONFIG 0x03
typedef struct {
    uint8_t auto_latch : 1;
} config_command_t;
#define EXPECTED_LENGTH_CONFIG 1

#define COMMAND_RESPONSE 0xFF
typedef struct {
    uint8_t status_code;
    uint8_t original_command;
} command_response_t;
