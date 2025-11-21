
#include "com_interface.h"
#include "stm32g0xx_hal_def.h"
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

static void decode_text_mode_command(com_interface_message_t* msg) {
    char *buffer = (char*)msg;

    char* token = strtok(buffer, TOKEN_SEPARATOR);
    
    for (size_t i = 0; i < COMMAND_TOKEN_COUNT; i++) {
        if (strcmp(token, command_tokens[i]) == 0) {
            msg->command = command_ids[i];
            break;
        }
    }

    token = strtok(NULL, TOKEN_SEPARATOR);
    size_t data_index = 0;
    while (token != NULL && data_index < COMMAND_BUFFER_SIZE - 2) {
        msg->data[data_index++] = (uint8_t)atoi(token);
        token = strtok(NULL, TOKEN_SEPARATOR);
    }
    msg->data_length = data_index;

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

    if (strcmp((char*)msg, TEXT_MODE_TOKEN) == 0) {
        com_interface->text_mode = 1;
        com_interface->rx_tail = (com_interface->rx_tail + 1) % RX_BUFFER_SIZE;
        CDC_Transmit_FS((uint8_t*)"Changing to text mode\n", strlen("Changing to text mode\n"));
        return COM_INTERFACE_OK;
    }
    if (strcmp((char*)msg, PACKET_MODE_TOKEN) == 0) {
        com_interface->text_mode = 0;
        com_interface->rx_tail = (com_interface->rx_tail + 1) % RX_BUFFER_SIZE;
        CDC_Transmit_FS((uint8_t*)"Changing to packet mode\n", strlen("Changing to packet mode\n"));
        return COM_INTERFACE_OK;
    }
    if (strcmp((char*)msg, HELP_TOKEN) == 0) {
        const char* help_text = 
            "Available Commands:\n"
            "TEXT_MODE - Switch to text mode\n"
            "PACKET_MODE - Switch to packet mode\n"
            "SET_PHASE_SHIFT <values> - Set phase shifter values\n"
            "PHASE_SHIFTER_LATCH <state> - Latch or unlatch phase shifters\n"
            "CONFIG <auto_latch>\n"
            "PING <data>\n";
        CDC_Transmit_FS((uint8_t*)help_text, strlen(help_text));
        com_interface->rx_tail = (com_interface->rx_tail + 1) % RX_BUFFER_SIZE;
        return COM_INTERFACE_OK;
    }

    if( com_interface->text_mode ) {
        decode_text_mode_command(msg);
    }
    
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
            UNUSED(cmd);
            phase_shifter_unlatch(&beam_controller.phase_shifter);
            break;
        }
        case COMMAND_CONFIG: {
            if (msg->data_length != 1) {
                status = COM_INTERFACE_ERROR;
            }
            config_command_t* cmd = (config_command_t*)msg->data;
            beam_controller.phase_shifter.auto_latch_enabled = cmd->auto_latch;
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

static void encode_text_mode_command(com_interface_message_t* msg, char* buffer, size_t buffer_size) {
    
    switch(msg->command) {
        case COMMAND_CONFIG:
            break;
        case COMMAND_PING:
            break;
        case COMMAND_RESPONSE: ;
            char* status_str = ((command_response_t*)msg->data)->status_code == STATUS_OK ? "OK" : "ERROR";
            char* original_command_str = "UNKNOWN";
            for (size_t i = 0; i < COMMAND_TOKEN_COUNT; i++) {
                if (command_ids[i] == ((command_response_t*)msg->data)->original_command) {
                    original_command_str = (char*)command_tokens[i];
                    break;
                }
            }
            snprintf(buffer, buffer_size, "%s %s %s\n", COMMAND_RESPONSE_TOKEN, status_str, original_command_str);
            break;
        default:
            snprintf(buffer, buffer_size, "UNKNOWN COMMAND: %d\n", msg->command);
            break;
    }
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

    char text_mode_buffer[256];

    if (com_interface->text_mode) {
        encode_text_mode_command(msg, text_mode_buffer, sizeof(text_mode_buffer));
        if (CDC_Transmit_FS((uint8_t*)text_mode_buffer, strlen(text_mode_buffer)) != USBD_OK) {
            return COM_INTERFACE_BUSY; // USB is busy
        }
    } else {
        if (CDC_Transmit_FS((uint8_t*)msg, msg->data_length + 2) != USBD_OK) {
            return COM_INTERFACE_BUSY; // USB is busy
        }
    }
    com_interface->tx_tail = (com_interface->tx_tail + 1) % TX_BUFFER_SIZE;

    return COM_INTERFACE_OK;
}
