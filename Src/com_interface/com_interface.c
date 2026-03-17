
#include "com_interface.h"
#include "com_interface/commands.h"

#include "util/circular_buffer.h"
#include "util/error_handle.h"

#include "usbd_cdc_if.h"

error_t com_interface_init(com_interface_t *com_interface)
{
    error_t err;

    err = circular_buffer_init(&com_interface->rx_buffer);
    if (err) return err;

    err = circular_buffer_init(&com_interface->tx_buffer);
    if (err) return err;

    com_interface_register_command(com_interface, CMD_PING, cmd_ping_handler, NULL);
    com_interface_register_command(com_interface, CMD_PONG, cmd_pong_handler, NULL);
    com_interface_register_command(com_interface, CMD_SET_PHASE_SHIFT, cmd_set_phase_shift_handler, NULL);
    com_interface_register_command(com_interface, CMD_TRANSMIT_PHASE_SHIFT, cmd_transmit_phase_shift_handler, NULL);
    com_interface_register_command(com_interface, CMD_SET_LATCH_STATE, cmd_set_latch_state_handler, NULL);

    com_interface->tx_endpoints[0] = CDC_Transmit_FS;

    return STATUS_OK;
}

error_t com_interface_register_command(com_interface_t *com_interface, uint8_t command_id, com_command_handler_t handler, void *context)
{
    if (command_id >= COM_COMMAND_COUNT) {
        return STATUS_ERROR;
    }

    com_interface->commands[command_id].handler = handler;
    com_interface->commands[command_id].context = context;

    return STATUS_OK;
}

error_t com_interface_process_rx(com_interface_t *com_interface)
{
    error_t err;
    message_t msg;

    if (circular_buffer_is_empty(com_interface->rx_buffer)) {
        return STATUS_OK; // No messages to process
    }

    if (circular_buffer_pop(&com_interface->rx_buffer, &msg) == STATUS_OK) {
        if (msg.id < COM_COMMAND_COUNT && com_interface->commands[msg.id].handler) {
            com_interface->commands[msg.id].handler(com_interface->commands[msg.id].context);
        }
    }

    return STATUS_OK;
}

error_t com_interface_process_tx(com_interface_t *com_interface)
{
    message_t msg;

    if (circular_buffer_is_empty(com_interface->tx_buffer)) {
        return STATUS_OK; // No messages to process
    }

    if (circular_buffer_pop(&com_interface->tx_buffer, &msg) == STATUS_OK) {
        for (int i = 0; i < ENDPOINT_COUNT; i++) {
            if (com_interface->tx_endpoints[i]) {
                com_interface->tx_endpoints[i](msg.data, msg.length);
            }
        }
    }

    return STATUS_OK;
}

error_t com_interface_add_rx_message(com_interface_t *com_interface, message_t msg)
{
    return circular_buffer_push(&com_interface->rx_buffer, msg);
}

error_t com_interface_add_tx_message(com_interface_t *com_interface, message_t msg)
{
    return circular_buffer_push(&com_interface->tx_buffer, msg);
}