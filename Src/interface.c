
#include "interface.h"
#include "main.h"
#include "phase_shifter.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include <string.h>

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

interface_status recieve_command(command *cmd, uint8_t *buffer, size_t size)
{
    interface_status status = INTERFACE_OK;

    if (size < 3) { // Minimum size is command + data size + CRC
        return INTERFACE_ERROR; // Invalid command size
    }

    cmd->command = buffer[0];

    cmd->data_size = buffer[1];

    memcpy(cmd->data, &buffer[2], cmd->data_size);

    cmd->crc = buffer[cmd->data_size + CRC_OFFSET]; // CRC is the last byte in the command

    return status;
}

interface_status process_command(command *cmd, command *response_cmd)
{
    interface_status status = INTERFACE_OK;
    
    memcpy(response_cmd, cmd, sizeof(command));

    response_cmd->command = COMMAND_ECHO;

    return status;
}
