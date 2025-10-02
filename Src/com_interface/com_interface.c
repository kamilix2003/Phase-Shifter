
#include "com_interface.h"
#include "stm32g0xx_hal_def.h"
#include "usbd_cdc_if.h"
#include "commands.h"
#include <string.h>
#include "../beam_controller.h"
#include "../phase_shifter/phase_shifter.h"

extern beam_controller_TypeDef beam_controller; // Assuming a single global instance for simplicity

com_interface_StatusTypeDef com_interface_init(com_interface_TypeDef* com_interface) {
    if (com_interface == NULL) {
        return COM_INTERFACE_ERROR;
    }

    memset(com_interface->rx_buffer, 0, sizeof(com_interface->rx_buffer));
    com_interface->rx_head = 0;
    com_interface->rx_tail = 0;

    memset(com_interface->tx_buffer, 0, sizeof(com_interface->tx_buffer));
    com_interface->tx_head = 0;
    com_interface->tx_tail = 0;

    return COM_INTERFACE_OK;
}

com_interface_StatusTypeDef com_interface_deinit(com_interface_TypeDef* com_interface) {
    if (com_interface == NULL) {
        return COM_INTERFACE_ERROR;
    }
    // Nothing to deinitialize for now
    return COM_INTERFACE_OK;
}

com_interface_StatusTypeDef com_interface_add_tx(com_interface_TypeDef* com_interface, com_interface_message_t* msg) {
    if (com_interface == NULL || msg == NULL || msg->data_length > 62) {
        return COM_INTERFACE_ERROR;
    }

    size_t next_head = (com_interface->tx_head + 1) % TX_BUFFER_SIZE;
    if (next_head == com_interface->tx_tail) {
        // Buffer is full
        return COM_INTERFACE_BUSY;
    }

    com_interface->tx_buffer[com_interface->tx_head] = *msg;
    com_interface->tx_head = next_head;

    return COM_INTERFACE_OK;
}

com_interface_StatusTypeDef com_interface_add_rx(com_interface_TypeDef* com_interface, uint8_t* buf, uint32_t len) {
    if (com_interface == NULL || buf == NULL || len < 2 || len > 64) {
        return COM_INTERFACE_ERROR;
    }

    size_t next_head = (com_interface->rx_head + 1) % RX_BUFFER_SIZE;
    if (next_head == com_interface->rx_tail) {
        // Buffer is full
        return COM_INTERFACE_BUSY;
    }

    com_interface_message_t* msg = &com_interface->rx_buffer[com_interface->rx_head];
    msg->command = buf[0];
    msg->data_length = buf[1];
    if (msg->data_length > 62 || msg->data_length != (len - 2)) {
        return COM_INTERFACE_ERROR; // Invalid data length
    }
    memcpy(msg->data, &buf[2], msg->data_length);

    com_interface->rx_head = next_head;

    return COM_INTERFACE_OK;
}

com_interface_StatusTypeDef com_interface_process_rx(com_interface_TypeDef* com_interface) {
    if (com_interface == NULL) {
        return COM_INTERFACE_ERROR;
    }

    if (com_interface->rx_head == com_interface->rx_tail) {
        // No new messages
        return COM_INTERFACE_OK;
    }

    com_interface_message_t* msg = &com_interface->rx_buffer[com_interface->rx_tail];
    
    switch (msg->command) {
        case COMMAND_SET_PHASE_SHIFT: {
            if (msg->data_length != PHASE_SHIFTER_COUNT) {
                return COM_INTERFACE_ERROR;
            }
            set_phase_shift_command_t* cmd = (set_phase_shift_command_t*)msg->data;
            phase_shifter_set(&beam_controller.phase_shifter, cmd->phase_shifter_values, PHASE_SHIFTER_COUNT);
            break;
        }
        case COMMAND_PHASE_SHIFTER_LATCH: {
            if (msg->data_length != 1) {
                return COM_INTERFACE_ERROR;
            }
            phase_shifter_latch_command_t* cmd = (phase_shifter_latch_command_t*)msg->data;
            UNUSED(cmd);
            phase_shifter_unlatch(&beam_controller.phase_shifter);
            break;
        }
        case COMMAND_CONFIG: {
            if (msg->data_length != 1) {
                return COM_INTERFACE_ERROR;
            }
            config_command_t* cmd = (config_command_t*)msg->data;
            beam_controller.phase_shifter.auto_latch_enabled = cmd->auto_latch;
            break;
        }
        default:
            return COM_INTERFACE_ERROR; // Unknown command
    }


    com_interface->rx_tail = (com_interface->rx_tail + 1) % RX_BUFFER_SIZE;

    return COM_INTERFACE_OK;
}

com_interface_StatusTypeDef com_interface_process_tx(com_interface_TypeDef* com_interface) {
    if (com_interface == NULL) {
        return COM_INTERFACE_ERROR;
    }

    if (com_interface->tx_head == com_interface->tx_tail) {
        // No messages to send
        return COM_INTERFACE_OK;
    }

    com_interface_message_t* msg = &com_interface->tx_buffer[com_interface->tx_tail];

    uint8_t buffer[64];
    buffer[0] = msg->command;
    buffer[1] = msg->data_length;
    memcpy(&buffer[2], msg->data, msg->data_length);

    uint16_t total_length = 2 + msg->data_length;
    if (CDC_Transmit_FS(buffer, total_length) != USBD_OK) {
        return COM_INTERFACE_BUSY; // USB is busy
    }

    com_interface->tx_tail = (com_interface->tx_tail + 1) % TX_BUFFER_SIZE;

    return COM_INTERFACE_OK;
}
