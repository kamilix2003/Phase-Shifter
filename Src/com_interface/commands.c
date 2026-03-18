
#include "beam_controller.h"
#include "com_interface.h"
#include "config.h"
#include "phase_shifter/phase_shifter_operations.h"
#include "util/circular_buffer.h"

#include "commands.h"

#include <string.h>

void cmd_ping_handler(const message_t* msg, void* ctx) {
    // Handle ping command
    message_t response;
    response.type = CMD_PONG;
    com_interface_add_rx_message(&controller.com_interface, response);
}

void cmd_pong_handler(const message_t* msg, void* ctx) {
    // Handle pong command (could be used for latency measurement or connection check)
}

void cmd_set_phase_shift_handler(const message_t* msg, void* ctx) {
    // Handle set phase shift command
    const cmd_set_phase_shift_t* cmd = (const cmd_set_phase_shift_t*)msg->data;
    phase_shifter_set_phase_context_t op_context;
    memcpy(op_context.phase_shifter_buffer, cmd->phase_shift, PHASE_SHIFTER_COUNT);
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, CMD_SET_PHASE_SHIFT, &op_context);
}

void cmd_transmit_phase_shift_handler(const message_t *msg, void *ctx) {
    // Handle transmit phase shift command
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, CMD_TRANSMIT_PHASE_SHIFT, NULL);
}

void cmd_set_latch_state_handler(const message_t *msg, void *ctx) {
    // Handle set latch state command
    const cmd_set_latch_state_t* cmd = (const cmd_set_latch_state_t*)msg->data;
    phase_shifter_set_latch_context_t op_context;
    op_context.latch_state = cmd->latch_state;
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, CMD_SET_LATCH_STATE, &op_context);
}