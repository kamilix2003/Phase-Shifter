#pragma once

#include "util/error_handle.h"
#include "util/soft_timer.h"

#include "phase_shifter/phase_shifter_manager.h"
#include "phase_shifter/phase_shifter_operations.h"

#define SOFT_TIMER_COUNT 8

typedef struct {

    phase_shifter_manager_t phase_shifter_manager;

    soft_timer_t soft_timers[SOFT_TIMER_COUNT];

} beam_controller_t;

typedef struct {

    SPI_HandleTypeDef* hspi;

} beam_controller_config_t;

error_t beam_controller_init(beam_controller_t* controller, const beam_controller_config_t* config);
error_t beam_controller_update(beam_controller_t* controller);