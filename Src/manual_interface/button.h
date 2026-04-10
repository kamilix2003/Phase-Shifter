#pragma once

#include "gpio.h"

#include "util/error_handle.h"
#include "util/soft_timer.h"
#include <stdint.h>

typedef void (*button_callback_t)(void* context);

typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_SHORT_PRESS,
    BUTTON_EVENT_LONG_PRESS
} button_event_t;

typedef struct {

    GPIO_TypeDef* button_port;
    uint32_t button_pin;

    soft_timer_t* debounce_timer;
    soft_timer_t* long_press_timer;

    uint8_t is_debouncing;
    button_event_t current_event;

    button_callback_t short_press_callback;
    button_callback_t long_press_callback;

} button_t;

typedef struct {

    GPIO_TypeDef* button_port;
    uint32_t button_pin;

    soft_timer_t* debounce_timer;
    uint32_t debounce_duration_ms;

    soft_timer_t* long_press_timer;
    uint32_t long_press_duration_ms;

    button_callback_t short_press_callback;
    button_callback_t long_press_callback;

} button_config_t;

error_t button_init(button_t* button, const button_config_t* config);
error_t button_update(button_t* button);