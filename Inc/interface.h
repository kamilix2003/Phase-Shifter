
#ifndef INTERFACE_H
#define INTERFACE_H

#include "phase_shifter.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_crc.h"

#include "usbd_cdc_if.h"
#include <sys/types.h>

#define CMD_BUFFER_SIZE 64
#define MAX_DATA_SIZE 60 // Maximum size of data that can be sent in a command

#define COMMAND_ADDR 0 // Offset for command in the command buffer
#define DATA_SIZE_ADDR 1 // Offset for data size in the command buffer
#define DATA_ADDR 2 // Offset for data in the command buffer

#define CRC_OFFSET 1 // Offset for CRC in the command buffer
#define END_OFFSET 2 // Offset for end of command in the command buffer

typedef enum {
    INTERFACE_OK = 0,
    INTERFACE_ERROR = -1,
    INTERFACE_BUSY = -2,
    INTERFACE_TIMEOUT = -3
}interface_status;

typedef struct 
{

    CRC_HandleTypeDef *hcrc; // CRC handle for error checking

}interface;

// COMMANDS

// Service commands
#define COMMAND_OK 0x00
#define COMMAND_ECHO 0x01
#define COMMAND_ERROR 0xff

// Settings commands
#define COMMAND_PHASE_SHIFTER_COUNT 0x03
#define COMMAND_AUTO_LATCH 0x04

// Buffer commands
#define COMMAND_SEND_BUFFER 0x10
#define COMMAND_SET_BUFFER 0x11
#define COMMAND_GET_BUFFER 0x12
#define COMMAND_GET_BUFFER_RE 0x13
#define COMMAND_CLEAR_BUFFER 0x14

typedef struct {

    uint8_t command;
    uint8_t * data;
    size_t data_size;
    uint8_t crc; // CRC for error checking, can be calculated based on data

}command;

interface_status interface_init(CRC_HandleTypeDef *hcrc);

interface_status transmit_command(command *cmd);

interface_status recieve_command(uint8_t *buffer, size_t size);

interface_status process_command(void);

#endif
