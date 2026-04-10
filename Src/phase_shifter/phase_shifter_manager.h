#pragma once

#include "spi.h"
#include "gpio.h"
#include "main.h"

#include "util/circular_buffer.h"
#include "util/error_handle.h"
#include "config.h"

#define OPERATION_CALLBACK_COUNT 32
#define IS_VALID_OPERATION_CODE(code) ((code) < OPERATION_CALLBACK_COUNT)

typedef struct phase_shifter_manager_t phase_shifter_manager_t;

typedef void (*operation_callback_t)(phase_shifter_manager_t* manager, void* context);

typedef struct {
    operation_callback_t callback;
    void* context;
} phase_shifter_operation_t;

struct phase_shifter_manager_t {
    
    SPI_HandleTypeDef* hspi;

    uint8_t phase_shifter_buffer[PHASE_SHIFTER_COUNT];
    
    uint8_t latch_state;

    circular_buffer_t operation_queue;

    phase_shifter_operation_t operation_callbacks[OPERATION_CALLBACK_COUNT];

};

error_t phase_shifter_manager_init(phase_shifter_manager_t* manager, SPI_HandleTypeDef* hspi);

error_t phase_shifter_manager_register_callback(phase_shifter_manager_t* manager, uint8_t operation_code, operation_callback_t callback, void* context);
error_t phase_shifter_manager_append_operation(phase_shifter_manager_t* manager, uint8_t operation_code, void* context);

error_t phase_shifter_manager_update(phase_shifter_manager_t* manager);

