#pragma once

#include "spi.h"
#include "gpio.h"
#include "main.h"

#include "util/error_handle.h"
#include "config.h"

typedef struct {

    uint8_t phase_shifter_buffer[PHASE_SHIFTER_COUNT];
    
    SPI_HandleTypeDef* hspi;

} phase_shifter_manager_t;

error_t phase_shifter_manager_init(phase_shifter_manager_t* manager, SPI_HandleTypeDef* hspi);
error_t phase_shifter_set_phase(phase_shifter_manager_t* manager, uint8_t* data, size_t length);
error_t phase_shifter_set_latch(phase_shifter_manager_t* manager, uint8_t latch_state);