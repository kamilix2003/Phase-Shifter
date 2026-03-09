#pragma once

#include "util/circular_buffer.h"
#include "util/error_handle.h"

#include "phase_shifter_manager.h"

typedef enum{
    OPERATION_TYPE_SET_PHASE = 0,
    OPERATION_TYPE_SET_LATCH
} operation_type_t;

typedef struct {
    uint8_t phase_data[PHASE_SHIFTER_COUNT]
} set_phase_operation_t;

typedef struct {
    uint8_t latch_state;
} set_latch_operation_t;

typedef struct {
    circular_buffer_t operation_queue;
} operation_queue_t;