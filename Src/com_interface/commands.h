
#include "../phase_shifter/phase_shifter.h"
#include <stdint.h>

#define STATUS_OK 0x00
#define STATUS_ERROR 0xFF

/*
    EXAMPLE USAGE:
        To set phase shifters:
            01 04 val1 val2 val3 val4

        to set latch:
            02 01 bits
            set:
                02 01 01  (set latch high)
                02 01 00  (set latch low)
            toggle:
                02 01 02  (toggle latch)
            auto latch:
                02 01 04  (enable auto latch)
                02 01 00  (disable auto latch)
*/

#define COMMAND_SET_PHASE_SHIFT 0x01
typedef struct {
    uint8_t phase_shifter_values[PHASE_SHIFTER_COUNT];
} set_phase_shift_command_t;

#define COMMAND_PHASE_SHIFTER_LATCH 0x02
typedef struct {
    uint8_t latch_state : 1;
    uint8_t toogle : 1;
    uint8_t reserved : 5;
    uint8_t auto_latch : 1;
} phase_shifter_latch_command_t;

#define COMMAND_PING 0xFE
typedef struct {
    uint8_t ping_data;
} ping_command_t;

/*
    ff 02 status_code original_command
    ff 02 00 01  -> success response to command 0x01
    ff 02 ff 01  -> error response to command 0x01
*/
#define COMMAND_RESPONSE 0xFF
typedef struct {
    uint8_t status_code;
    uint8_t original_command;
} command_response_t;
