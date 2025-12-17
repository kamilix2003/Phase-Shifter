#pragma once

#include "../../Core/Inc/spi.h"
#include "../../Core/Inc/gpio.h"

#define PHASE_SHIFTER_COUNT 4
#define PHASE_SHIFTER_BITS 4

typedef struct{
    SPI_HandleTypeDef *hspi;
    
    GPIO_TypeDef *latch_port;
    uint16_t latch_pin;
    uint8_t latch_state;

    uint8_t auto_latch_enabled;

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
phase_shifter_StatusTypeDef phase_shifter_set(phase_shifter_TypeDef *hps, uint8_t *data, size_t size);
phase_shifter_StatusTypeDef phase_shifter_latch(phase_shifter_TypeDef *hps, uint8_t latch_state);
phase_shifter_StatusTypeDef phase_shifter_unlatch(phase_shifter_TypeDef *hps);

