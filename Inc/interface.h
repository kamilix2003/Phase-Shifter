
#ifndef INTERFACE_H
#define INTERFACE_H

#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_crc.h"

#include "usbd_cdc_if.h"
#include <sys/types.h>

#define CMD_BUFFER_SIZE 64
#define MAX_DATA_SIZE 60 // Maximum size of data that can be sent in a command
#define COMMAND_ADDR 0 // Offset for command in the command buffer
#define DATA_SIZE_ADDR 1 // Offset for data size in the command buffer
#define CRC_OFFSET 1 // Offset for CRC in the command buffer
#define END_OFFSET 2 // Offset for end of command in the command buffer

typedef enum {
    INTERFACE_OK = 0,
    INTERFACE_ERROR = -1,
    INTERFACE_BUSY = -2,
    INTERFACE_TIMEOUT = -3
}interface_status;

#define COMMAND_ECHO 0x00
#define COMMAND_ERROR 0x01

#define COMMAND_SET_BUFFER 0x11
#define COMMAND_GET_BUFFER 0x12
#define COMMAND_CLEAR_BUFFER 0x13

#define COMMAND_SET_VALUE 0x24
#define COMMAND_GET_VALUE 0x25

typedef struct {

    uint8_t command;
    uint8_t * data;
    size_t data_size;
    uint8_t crc; // CRC for error checking, can be calculated based on data

}command;

interface_status transmit_command(command *cmd);

interface_status recieve_command(command *cmd, uint8_t *buffer, size_t size);

interface_status process_command(command *cmd, command *response_cmd);

#endif
