
#include "com_interface.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_uart.h"
#include "usbd_cdc_if.h"
#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
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
    if (com_interface == NULL || buf == NULL || len > COMMAND_BUFFER_SIZE) {
        return COM_INTERFACE_ERROR;
    }

    size_t next_head = (com_interface->rx_head + 1) % RX_BUFFER_SIZE;
    if (next_head == com_interface->rx_tail) {
        // Buffer is full
        return COM_INTERFACE_BUSY;
    }

    com_interface_message_t* msg = &com_interface->rx_buffer[com_interface->rx_head];
    memcpy(msg, buf, len);

    com_interface->rx_head = next_head;

    return COM_INTERFACE_OK;
}

static void add_response_message(com_interface_message_t* msg, com_interface_StatusTypeDef status) {

    com_interface_message_t response;
    response.command = COMMAND_RESPONSE;
    response.data_length = 2;
    command_response_t* resp_data = (command_response_t*)response.data;
    resp_data->status_code = (status == COM_INTERFACE_OK) ? STATUS_OK : STATUS_ERROR;
    resp_data->original_command = msg->command;
    com_interface_add_tx(&beam_controller.com_interface, &response);

}

com_interface_StatusTypeDef com_interface_process_rx(com_interface_TypeDef* com_interface) {
    if (com_interface == NULL) {
        return COM_INTERFACE_ERROR;
    }

    if (com_interface->rx_head == com_interface->rx_tail) {
        // No new messages
        return COM_INTERFACE_OK;
    }

    com_interface_StatusTypeDef status = COM_INTERFACE_OK;

    com_interface_message_t* msg = &com_interface->rx_buffer[com_interface->rx_tail];

    switch (msg->command) {
        case COMMAND_SET_PHASE_SHIFT: {
            if (msg->data_length != PHASE_SHIFTER_COUNT) {
                status = COM_INTERFACE_ERROR;
            }
            set_phase_shift_command_t* cmd = (set_phase_shift_command_t*)msg->data;
            status = (com_interface_StatusTypeDef)phase_shifter_set(&beam_controller.phase_shifter, cmd->phase_shifter_values, PHASE_SHIFTER_COUNT);
            break;
        }
        case COMMAND_PHASE_SHIFTER_LATCH: {
            if (msg->data_length != 1) {
                status = COM_INTERFACE_ERROR;
            }
            phase_shifter_latch_command_t* cmd = (phase_shifter_latch_command_t*)msg->data;
            
            if (cmd->toogle == 0 && cmd->latch_state == 0) {
                beam_controller.phase_shifter.auto_latch_enabled = cmd->auto_latch;
            }
            
            if (cmd->auto_latch == 0 && cmd->toogle == 1) {
                status = (com_interface_StatusTypeDef)phase_shifter_latch(&beam_controller.phase_shifter, !beam_controller.phase_shifter.latch_state);
                beam_controller.phase_shifter.latch_state = !beam_controller.phase_shifter.latch_state;
            }
            else {
                status = (com_interface_StatusTypeDef)phase_shifter_latch(&beam_controller.phase_shifter, cmd->latch_state);
            }

            break;
        }
        case COMMAND_PING: {
            if (msg->data_length != 1) {
                status = COM_INTERFACE_ERROR;
            }

            com_interface_message_t response;
            response.command = COMMAND_RESPONSE;
            response.data_length = 2;

            command_response_t* resp_data = (command_response_t*)response.data;
            resp_data->status_code = STATUS_OK;
            resp_data->original_command = COMMAND_PING;

            status = com_interface_add_tx(com_interface, &response);
            break;
        }
        default:
            status = COM_INTERFACE_ERROR; // Unknown command
    }


    com_interface->rx_tail = (com_interface->rx_tail + 1) % RX_BUFFER_SIZE;

    add_response_message(msg, status);

    return status;
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

    if (CDC_Transmit_FS((uint8_t*)msg, msg->data_length + 2) != USBD_OK) {
        return COM_INTERFACE_BUSY; // USB is busy
    }

    HAL_UART_Transmit(com_interface->huart, (uint8_t*)msg, msg->data_length + 2, HAL_MAX_DELAY);

    com_interface->tx_tail = (com_interface->tx_tail + 1) % TX_BUFFER_SIZE;

    return COM_INTERFACE_OK;
}
