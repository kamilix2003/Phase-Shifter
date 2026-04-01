
#include "beam_controller.h"
#include "com_interface.h"
#include "config.h"
#include "phase_shifter/phase_shifter_operations.h"
#include "util/circular_buffer.h"

#include "commands.h"

#include <string.h>

static inline void append_ack_message(com_interface_t* com_interface, uint8_t command_id) {
    
    message_t ack_msg;
    ack_msg.type = CMD_ACK;

    cmd_ack_t ack_payload = { .command_id = command_id };
    memcpy(ack_msg.data, &ack_payload, sizeof(cmd_ack_t));

    ack_msg.length = sizeof(cmd_ack_t);

    com_interface_add_tx_message(com_interface, ack_msg);
}

void cmd_ping_handler(const message_t* msg, void* ctx) {
    // Handle ping command
    message_t response;
    response.type = CMD_PONG;
    com_interface_add_tx_message(&controller.com_interface, response);
}

void cmd_pong_handler(const message_t* msg, void* ctx) {
    // Handle pong command (could be used for latency measurement or connection check)
}

void cmd_set_phase_shift_handler(const message_t* msg, void* ctx) {
    // Handle set phase shift command
    const cmd_set_phase_shift_t* cmd = (const cmd_set_phase_shift_t*)msg->data;
    phase_shifter_set_phase_context_t op_context;
    memcpy(op_context.phase_shifter_buffer, cmd->phase_shift, PHASE_SHIFTER_COUNT);
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_PHASE_CODE, &op_context);

    append_ack_message(&controller.com_interface, CMD_SET_PHASE_SHIFT);
}

void cmd_transmit_phase_shift_handler(const message_t *msg, void *ctx) {
    // Handle transmit phase shift command
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_TRANSMIT_CODE, NULL);
    
    append_ack_message(&controller.com_interface, CMD_TRANSMIT_PHASE_SHIFT);
}

void cmd_set_latch_state_handler(const message_t *msg, void *ctx) {
    // Handle set latch state command
    const cmd_set_latch_state_t* cmd = (const cmd_set_latch_state_t*)msg->data;
    phase_shifter_set_latch_context_t op_context;
    op_context.latch_state = cmd->latch_state;
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_LATCH_CODE, &op_context);
    
    append_ack_message(&controller.com_interface, CMD_SET_LATCH_STATE);
}