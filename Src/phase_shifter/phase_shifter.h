#pragma once

#include "../../Core/Inc/spi.h"
#include "../../Core/Inc/gpio.h"
#include <stdint.h>

#define PHASE_SHIFTER_COUNT 4
#define PHASE_SHIFTER_BITS 4

#define MAX_ARG_SIZE 16
#define COMMAND_BUFFER_SIZE 16

typedef enum {
    PHASE_SHIFTER_OP_CODE_SET = 0x01,
    PHASE_SHIFTER_OP_CODE_LATCH = 0x02,
    PHASE_SHIFTER_OP_CODE_UNLATCH = 0x03
}phase_shifter_op_code_t;

typedef struct {
    uint8_t op_code;
    uint8_t arg_size;
    uint8_t args[MAX_ARG_SIZE];
} phase_shifter_command_t;

typedef struct{
    SPI_HandleTypeDef *hspi;
    
    GPIO_TypeDef *latch_port;
    uint16_t latch_pin;
    uint8_t latch_state;

    uint8_t auto_latch_enabled;

    phase_shifter_command_t command_buffer[COMMAND_BUFFER_SIZE];
    uint8_t command_buffer_head;
    uint8_t command_buffer_tail;

    uint8_t buffer[PHASE_SHIFTER_COUNT];
}phase_shifter_TypeDef;

typedef enum {
    PHASE_SHIFTER_OK = 0,
    PHASE_SHIFTER_ERROR = -1,
    PHASE_SHIFTER_BUSY = -2,
    PHASE_SHIFTER_TIMEOUT = -3
}phase_shifter_StatusTypeDef;

phase_shifter_StatusTypeDef phase_shifter_init(phase_shifter_TypeDef *hps, SPI_HandleTypeDef *hspi, GPIO_TypeDef *latch_port, uint16_t latch_pin);
phase_shifter_StatusTypeDef phase_shifter_deinit(phase_shifter_TypeDef *hps);

phase_shifter_StatusTypeDef phase_shifter_process_commands(phase_shifter_TypeDef *hps);
phase_shifter_StatusTypeDef phase_shifter_enqueue_command(phase_shifter_TypeDef *hps, phase_shifter_command_t *command);

phase_shifter_StatusTypeDef phase_shifter_set(phase_shifter_TypeDef *hps, uint8_t *data, size_t size);
phase_shifter_StatusTypeDef phase_shifter_latch(phase_shifter_TypeDef *hps, uint8_t latch_state);
phase_shifter_StatusTypeDef phase_shifter_unlatch(phase_shifter_TypeDef *hps);

