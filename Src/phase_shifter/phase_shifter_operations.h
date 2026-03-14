#pragma once

#include "config.h"
#include "phase_shifter_manager.h"
#include <stddef.h>

#define PHASE_SHIFTER_SET_PHASE_CODE 0x01
typedef struct {
    uint8_t phase_shifter_buffer[PHASE_SHIFTER_COUNT];
} phase_shifter_set_phase_context_t;
operation_callback_t phase_shifter_set_phase(phase_shifter_manager_t* manager, void* context);

#define PHASE_SHIFTER_TRANSMIT_CODE 0x02
typedef struct {
    // No additional context needed for transmit operation
} phase_shifter_transmit_context_t;
operation_callback_t phase_shifter_transmit(phase_shifter_manager_t* manager, void* context);

#define PHASE_SHIFTER_SET_LATCH_CODE 0x03
typedef struct {
    uint8_t latch_state;
} phase_shifter_set_latch_context_t;
operation_callback_t phase_shifter_set_latch(phase_shifter_manager_t* manager, void* context);

static const size_t phase_shifter_operation_context_size_map[OPERATION_CALLBACK_COUNT] = {
    [PHASE_SHIFTER_SET_PHASE_CODE] = sizeof(phase_shifter_set_phase_context_t),
    [PHASE_SHIFTER_TRANSMIT_CODE] = sizeof(phase_shifter_transmit_context_t),
    [PHASE_SHIFTER_SET_LATCH_CODE] = sizeof(phase_shifter_set_latch_context_t)
};

size_t get_phase_shifter_operation_context_size(uint8_t operation_code);