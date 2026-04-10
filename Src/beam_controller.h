#pragma once

#include "com_interface/com_interface.h"
#include "util/error_handle.h"
#include "util/soft_timer.h"

#include "phase_shifter/phase_shifter_manager.h"
#include "phase_shifter/phase_shifter_operations.h"

#include "manual_interface/button_interface.h"

#define SOFT_TIMER_COUNT 8

typedef struct {

    phase_shifter_manager_t phase_shifter_manager;

    button_interface_t button_interface;

    com_interface_t com_interface;

    soft_timer_t soft_timers[SOFT_TIMER_COUNT];

} beam_controller_t;

extern beam_controller_t controller;

typedef struct {

    SPI_HandleTypeDef* hspi;

} beam_controller_config_t;

error_t beam_controller_init(beam_controller_t* controller, const beam_controller_config_t* config);
error_t beam_controller_update(beam_controller_t* controller);