#pragma once

#include "manual_interface/led_indicator.h"
#include "manual_interface/button.h"

#include "util/error_handle.h"
#include "util/soft_timer.h"
#include <stdint.h>

typedef struct{

    LED_indicator phase_shifter_select_indicator;
    LED_indicator phase_shifter_state_indicator;

    button_t phase_shifter_select_button;
    button_t phase_shifter_state_button;
    button_t latch_button;

    uint8_t selected_phase_shifter;

} button_interface_t;

error_t button_interface_init(button_interface_t* interface);
error_t button_interface_update(button_interface_t* interface);