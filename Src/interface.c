
#include "interface.h"
#include "main.h"
#include "phase_shifter.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <string.h>
#include <sys/_intsup.h>

command cmd;
command response_cmd;

interface intf;

static uint8_t interface_buffer[CMD_BUFFER_SIZE];
static uint8_t responese_buffer[CMD_BUFFER_SIZE];

interface_status interface_init(phase_shifter *ps)
{
    interface_status status = INTERFACE_OK;

    intf.ps = ps;

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
        memcpy(&buffer[DATA_SIZE_ADDR + 1], cmd->data, cmd->data_size);
    }

    buffer[cmd->data_size + CRC_OFFSET] = cmd->crc; // Reserved byte, can be used for future extensions

    buffer[cmd->data_size + END_OFFSET] = '\0'; // Null-terminate the command

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

    cmd.crc = buffer[DATA_ADDR + cmd.data_size + CRC_OFFSET]; // CRC is the last byte in the command

    return status;
}

interface_status process_command(void)
{
    interface_status status = INTERFACE_OK;
    
    response_cmd.command = COMMAND_OK;
    memset(response_cmd.data, 0, MAX_DATA_SIZE);
    response_cmd.data_size = 0;
    response_cmd.crc = 0;

    uint8_t error = 0;

    switch (cmd.command) {
        case COMMAND_ECHO:
            transmit_command(&cmd);
            break;

        case COMMAND_SPI_BUFFER_SIZE:
            if ( cmd.data[0] != 0)
                intf.ps->phase_shifter_count = cmd.data[0];
            else 
                response_cmd.command = COMMAND_ERROR;
            transmit_command(&response_cmd);
            break;

        case COMMAND_SET_BUFFER:
            if ( cmd.data_size > BUFFER_SIZE){
                response_cmd.command = COMMAND_ERROR;
                transmit_command(&response_cmd);
                break;
            }

            for (uint8_t i = 0; i < cmd.data_size; i++)
            {
                if (cmd.data[i] > PHASE_SHIFTER_MAX_VALUE)
                {
                    error = 1;
                    break;
                }
            }

            if (error){
                response_cmd.command = COMMAND_ERROR;
                transmit_command(&response_cmd);
                break;
            }

            memcpy(intf.ps->buffer, cmd.data, cmd.data_size);

            transmit_command(&response_cmd);
            break;

        case COMMAND_GET_BUFFER:

            response_cmd.command = COMMAND_GET_BUFFER_RE;
            response_cmd.data_size = BUFFER_SIZE;
            memcpy(response_cmd.data, intf.ps->buffer, response_cmd.data_size);

            transmit_command(&response_cmd);

            break;

        case COMMAND_CLEAR_BUFFER:

            memset(intf.ps->buffer, 0, BUFFER_SIZE);

            transmit_command(&response_cmd);

            break;

        default:
            status = INTERFACE_ERROR; // Unknown command
    }

    return status;
}
