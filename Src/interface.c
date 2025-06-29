
#include "interface.h"
#include "main.h"
#include "phase_shifter.h"
#include "stm32g0xx_hal_crc.h"
#include "stm32g0xx_hal_gpio.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>

command cmd;
command response_cmd;

interface intf;

static uint8_t interface_buffer[CMD_BUFFER_SIZE];
static uint8_t responese_buffer[CMD_BUFFER_SIZE];

interface_status interface_init(phase_shifter *ps, CRC_HandleTypeDef *hcrc)
{
    interface_status status = INTERFACE_OK;

    intf.ps = ps;
    intf.hcrc = hcrc; // Assign the CRC handle to the phase shifter

    cmd.command = COMMAND_ECHO; // Initialize command with ECHO command
    cmd.data = interface_buffer; // No data for ECHO command
    cmd.data_size = 0; // No data size for ECHO command
    cmd.crc = 0; // Initialize CRC to 0, can be calculated later

    response_cmd.command = COMMAND_ECHO; // Initialize response command with ECHO command
    response_cmd.data = responese_buffer; // No data for ECHO command
    response_cmd.data_size = 0; // No data size for ECHO command
    response_cmd.crc = 0; // Initialize CRC to 0, can be calculated later

    return status;
}

interface_status transmit_command(command *cmd)
{
    interface_status status = INTERFACE_OK;
    
    static uint8_t buffer[CMD_BUFFER_SIZE];

    buffer[COMMAND_ADDR] = cmd->command;

    buffer[DATA_SIZE_ADDR] = cmd->data_size; // Store the size of the data

    if (cmd->data_size > 0 && cmd->data != NULL) {
        if (cmd->data_size > MAX_DATA_SIZE) {
            return INTERFACE_ERROR; // Buffer overflow
        }
        memcpy(&buffer[DATA_ADDR], cmd->data, cmd->data_size);
    }

    cmd->crc = (uint8_t)HAL_CRC_Calculate(intf.hcrc, (uint32_t*)buffer, DATA_ADDR - 1 + cmd->data_size);

    buffer[DATA_ADDR - 1 + cmd->data_size + CRC_OFFSET] = cmd->crc; // Reserved byte, can be used for future extensions

    buffer[DATA_ADDR - 1 + cmd->data_size + END_OFFSET] = '\0'; // Null-terminate the command

    CDC_Transmit_FS(buffer, sizeof(buffer));

    return status;
}

interface_status recieve_command(uint8_t *buffer, size_t size)
{
    interface_status status = INTERFACE_OK;

    if (size < 3) { // Minimum size is command + data size + CRC
        return INTERFACE_ERROR; // Invalid command size
    }

    cmd.command = buffer[0];

    cmd.data_size = buffer[1];

    memcpy(cmd.data, &(buffer[2]), cmd.data_size);

    cmd.crc = buffer[DATA_ADDR - 1 + cmd.data_size + CRC_OFFSET]; // CRC is the last byte in the command

    uint8_t expected_crc = (uint8_t)HAL_CRC_Calculate(intf.hcrc, (uint32_t*)buffer, DATA_ADDR + cmd.data_size);
    if (cmd.crc != expected_crc && cmd.command != COMMAND_ECHO) {
        printf("CRC mismatch: expected %02X, got %02X\n", cmd.crc, expected_crc);
        return INTERFACE_ERROR; // CRC mismatch
    }

    return status;
}

interface_status process_command(void)
{
    interface_status status = INTERFACE_OK;
    
    response_cmd.command = COMMAND_OK;
    memset(response_cmd.data, 0, MAX_DATA_SIZE + END_OFFSET); // Clear the response data buffer
    response_cmd.data_size = 0;
    response_cmd.crc = 0;

    switch (cmd.command) {
        case COMMAND_ECHO:
            transmit_command(&cmd);
            break;

        case COMMAND_PHASE_SHIFTER_COUNT:
            if (phase_shifter_set_count(cmd.data[0]) != PHASE_SHIFTER_OK) {
                response_cmd.command = COMMAND_ERROR;
                transmit_command(&response_cmd);
                return INTERFACE_ERROR; // Error setting phase shifter count
            }
            transmit_command(&response_cmd);
            break;

        case COMMAND_SEND_BUFFER:

            if (phase_shifter_send() == PHASE_SHIFTER_OK) {
                response_cmd.command = COMMAND_OK;
            } else {
                response_cmd.command = COMMAND_ERROR;
            }
            transmit_command(&response_cmd);
            break;

        case COMMAND_SET_BUFFER:
            if (phase_shifter_set_buffer(cmd.data, cmd.data_size) != PHASE_SHIFTER_OK) {
                response_cmd.command = COMMAND_ERROR;
                transmit_command(&response_cmd);
                return INTERFACE_ERROR; // Error setting buffer
            }

            transmit_command(&response_cmd);
            break;

        case COMMAND_GET_BUFFER:

            response_cmd.command = COMMAND_GET_BUFFER_RE;
            phase_shifter_get_buffer(response_cmd.data, &response_cmd.data_size);

            transmit_command(&response_cmd);

            break;

        case COMMAND_CLEAR_BUFFER:

            if (phase_shifter_clear_buffer() != PHASE_SHIFTER_OK) {
                response_cmd.command = COMMAND_ERROR;
                transmit_command(&response_cmd);
                return INTERFACE_ERROR; // Error clearing buffer
            }

            transmit_command(&response_cmd);

            break;

        default:
            status = INTERFACE_ERROR; // Unknown command
    }

    return status;
}
