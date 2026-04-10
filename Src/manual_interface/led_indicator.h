#pragma once

#include "gpio.h"

#include "phase_shifter/phase_shifter_manager.h"

#include "util/error_handle.h"

#define LED_COUNT 4

typedef struct {

    GPIO_TypeDef* LED_port[LED_COUNT];
    uint32_t LED_pins[LED_COUNT];

    uint8_t LED_states[LED_COUNT];

} LED_indicator;

error_t LED_indicator_init(LED_indicator* indicator, GPIO_TypeDef* LED_ports[LED_COUNT], uint32_t LED_pins[LED_COUNT]);
error_t LED_indicator_update(LED_indicator* indicator);

error_t LED_indicator_set_state(LED_indicator* indicator, uint8_t led_index, uint8_t state);
error_t LED_indicator_set_state_all(LED_indicator* indicator, uint8_t state[LED_COUNT]);
error_t LED_indicator_toggle_state(LED_indicator* indicator, uint8_t led_index);
error_t LED_indicator_toggle_all(LED_indicator* indicator);
error_t LED_indicator_reset_all(LED_indicator* indicator);
error_t LED_indicator_set_all(LED_indicator* indicator);
error_t LED_indicator_set_binary(LED_indicator* indicator, uint8_t value);