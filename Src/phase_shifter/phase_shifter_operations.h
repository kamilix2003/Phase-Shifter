#pragma once

#include "config.h"
#include "phase_shifter_manager.h"

typedef struct {
    uint8_t phase_shifter_buffer[MAX_PHASE_SHIFTER_COUNT];
    size_t buffer_length;
} phase_shifter_set_phase_context_t;
operation_callback_t phase_shifter_set_phase(phase_shifter_manager_t* manager, void* context);

typedef struct {
    // No additional context needed for transmit operation
} phase_shifter_transmit_context_t;
operation_callback_t phase_shifter_transmit(phase_shifter_manager_t* manager, void* context);

typedef struct {
    uint8_t latch_state;
} phase_shifter_set_latch_context_t;
operation_callback_t phase_shifter_set_latch(phase_shifter_manager_t* manager, void* context);