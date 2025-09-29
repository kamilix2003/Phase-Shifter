#pragma once

#include "com_interface/com_interface.h"
#include "phase_shifter/phase_shifter.h"

typedef struct {
    com_interface_TypeDef com_interface;
    phase_shifter_TypeDef phase_shifter;
} beam_controller_TypeDef;

typedef enum {
    BEAM_CONTROLLER_OK = 0,
    BEAM_CONTROLLER_ERROR = -1
} beam_controller_StatusTypeDef;

beam_controller_StatusTypeDef beam_controller_init(beam_controller_TypeDef* controller, SPI_HandleTypeDef* hspi, DMA_HandleTypeDef* hdma, GPIO_TypeDef* latch_port, uint16_t latch_pin);
beam_controller_StatusTypeDef beam_controller_deinit(beam_controller_TypeDef* controller);

beam_controller_StatusTypeDef beam_controller_update(beam_controller_TypeDef* controller);
