
#include "button.h"

#include "util/error_handle.h"
#include "util/soft_timer.h"

static void button_debounce_timer_callback(void* context) {
    button_t* button = (button_t*)context;

    soft_timer_stop(button->debounce_timer);
    soft_timer_reset(button->debounce_timer);

    button->is_debouncing = 0;
}

static void button_long_press_timer_callback(void* context) {
    button_t* button = (button_t*)context;

    soft_timer_stop(button->long_press_timer);
    soft_timer_reset(button->long_press_timer);

    button->current_event = BUTTON_EVENT_LONG_PRESS;
}

error_t button_init(button_t* button, const button_config_t* config) {
    if (!button || !config) {
        return STATUS_ERROR;
    }

    button->button_port = config->button_port;
    button->button_pin = config->button_pin;

    button->is_debouncing = 0;
    button->current_event = BUTTON_EVENT_NONE;

    button->short_press_callback = config->short_press_callback;
    button->long_press_callback = config->long_press_callback;
    
    button->debounce_timer = config->debounce_timer;
    soft_timer_config_t debounce_timer_config = {
        .period_ms = config->debounce_duration_ms,
        .auto_run = 1,
        .callback = button_debounce_timer_callback,
        .context = button,
        .is_periodic = 0
    };
    soft_timer_init(button->debounce_timer, &debounce_timer_config);

    button->long_press_timer = config->long_press_timer;
    soft_timer_config_t long_press_timer_config = {
        .period_ms = config->long_press_duration_ms,
        .auto_run = 1,
        .callback = button_long_press_timer_callback,
        .context = button,
        .is_periodic = 0
    };
    soft_timer_init(button->long_press_timer, &long_press_timer_config);
    
    return STATUS_OK;
}

error_t button_update(button_t* button) {
    if (!button) {
        return STATUS_ERROR;
    }

    switch (button->current_event) {
        case BUTTON_EVENT_SHORT_PRESS:
            button->short_press_callback(&button);
            break;
        case BUTTON_EVENT_LONG_PRESS:
            button->long_press_callback(&button);
            break;
        default:
            // Handle no event
            break;
    }

    button->current_event = BUTTON_EVENT_NONE;

    return STATUS_OK;
}