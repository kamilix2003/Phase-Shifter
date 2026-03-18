
#include "button_interface.h"
#include "beam_controller.h"
#include "led_indicator.h"
#include "config.h"
#include "phase_shifter/phase_shifter_manager.h"
#include "phase_shifter/phase_shifter_operations.h"

#include "stm32g0b1xx.h"
#include "stm32g0xx_hal_gpio.h"

#include <string.h>

#include "util/circular_buffer.h"
#include "util/error_handle.h"
#include "util/soft_timer.h"

#include "main.h"

static void phase_shifter_select_short_press_callback(void* context) {
    button_interface_t* interface = &controller.button_interface;
    interface->selected_phase_shifter = (interface->selected_phase_shifter + 1) % PHASE_SHIFTER_COUNT;

    interface->phase_shifter_select_button.current_event = BUTTON_EVENT_NONE;
}

static void phase_shifter_select_long_press_callback(void* context) {
    button_interface_t* interface = &controller.button_interface;

    phase_shifter_set_phase_context_t ctx;
    memset(ctx.phase_shifter_buffer, 0, PHASE_SHIFTER_COUNT);

    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_PHASE_CODE, &ctx);

    interface->phase_shifter_select_button.current_event = BUTTON_EVENT_NONE;
}

static void phase_shifter_state_short_press_callback(void* context) {
    
    button_interface_t* interface = &controller.button_interface;
    
    phase_shifter_set_phase_context_t ctx;
    memcpy(ctx.phase_shifter_buffer, controller.phase_shifter_manager.phase_shifter_buffer, PHASE_SHIFTER_COUNT);
    ctx.phase_shifter_buffer[interface->selected_phase_shifter] = (ctx.phase_shifter_buffer[interface->selected_phase_shifter] + 1) % (1 << PHASE_SHIFTER_BIT_RESOLUTION);
    
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_PHASE_CODE, &ctx);

    interface->phase_shifter_state_button.current_event = BUTTON_EVENT_NONE;
}

static void phase_shifter_state_long_press_callback(void* context) {
    button_interface_t* interface = &controller.button_interface;

    phase_shifter_set_phase_context_t ctx;
    memcpy(ctx.phase_shifter_buffer, controller.phase_shifter_manager.phase_shifter_buffer, PHASE_SHIFTER_COUNT);
    ctx.phase_shifter_buffer[interface->selected_phase_shifter] = 0;
    
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_PHASE_CODE, &ctx);

    interface->phase_shifter_state_button.current_event = BUTTON_EVENT_NONE;
}

static void latch_button_short_press_callback(void* context) {

    phase_shifter_set_latch_context_t ctx;

    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_TRANSMIT_CODE, NULL);
    ctx.latch_state = 1;
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_LATCH_CODE, &ctx);
    ctx.latch_state = 0;
    phase_shifter_manager_append_operation(&controller.phase_shifter_manager, PHASE_SHIFTER_SET_LATCH_CODE, &ctx);

    controller.button_interface.latch_button.current_event = BUTTON_EVENT_NONE;
}

error_t button_interface_init(button_interface_t* interface) {
    // Initialize LED indicators
    static const GPIO_TypeDef* select_ports[LED_COUNT] = {
        LED_PS_SELECT_3_GPIO_Port,
        LED_PS_SELECT_2_GPIO_Port,
        LED_PS_SELECT_1_GPIO_Port,
        LED_PS_SELECT_0_GPIO_Port
    };
    static const uint32_t select_pins[LED_COUNT] = {
        LED_PS_SELECT_3_Pin,
        LED_PS_SELECT_2_Pin,
        LED_PS_SELECT_1_Pin,
        LED_PS_SELECT_0_Pin
    };
    if (LED_indicator_init(&interface->phase_shifter_select_indicator, select_ports, select_pins) != STATUS_OK) {
        return STATUS_ERROR;
    }
    static const GPIO_TypeDef* state_ports[LED_COUNT] = {
        LED_PS_STATE_3_GPIO_Port,
        LED_PS_STATE_2_GPIO_Port,
        LED_PS_STATE_1_GPIO_Port,
        LED_PS_STATE_0_GPIO_Port
    };
    static const uint32_t state_pins[LED_COUNT] = {
        LED_PS_STATE_3_Pin,
        LED_PS_STATE_2_Pin,
        LED_PS_STATE_1_Pin,
        LED_PS_STATE_0_Pin
    };
    if (LED_indicator_init(&interface->phase_shifter_state_indicator, state_ports, state_pins) != STATUS_OK) {
        return STATUS_ERROR;
    }

    interface->selected_phase_shifter = 0;

    // Initialize buttons
    button_config_t select_button_config = {
        .button_port = PS_SELECT_GPIO_Port,
        .button_pin = PS_SELECT_Pin,
        .debounce_timer = &controller.soft_timers[PS_SELECT_BUTTON_DEBOUNCE_TIMER_ID],
        .long_press_timer = &controller.soft_timers[PS_SELECT_BUTTON_LONG_PRESS_TIMER_ID],
        .debounce_duration_ms = DEBOUNCE_DURATION_MS,
        .long_press_duration_ms = LONG_PRESS_DURATION_MS,
        .short_press_callback = phase_shifter_select_short_press_callback,
        .long_press_callback = phase_shifter_select_long_press_callback
    };
    if (button_init(&interface->phase_shifter_select_button, &select_button_config) != STATUS_OK) {
        return STATUS_ERROR;
    }
    button_config_t state_button_config = {
        .button_port = PS_PHASE_GPIO_Port,
        .button_pin = PS_PHASE_Pin,
        .debounce_timer = &controller.soft_timers[PS_PHASE_BUTTON_DEBOUNCE_TIMER_ID],
        .long_press_timer = &controller.soft_timers[PS_PHASE_BUTTON_LONG_PRESS_TIMER_ID],
        .debounce_duration_ms = DEBOUNCE_DURATION_MS,
        .long_press_duration_ms = LONG_PRESS_DURATION_MS,
        .short_press_callback = phase_shifter_state_short_press_callback,
        .long_press_callback = phase_shifter_state_long_press_callback
    };
    if (button_init(&interface->phase_shifter_state_button, &state_button_config) != STATUS_OK) {
        return STATUS_ERROR;
    }
    button_config_t latch_button_config = {
        .button_port = PS_LATCH_GPIO_Port,
        .button_pin = PS_LATCH_Pin,
        .debounce_timer = &controller.soft_timers[PS_LATCH_BUTTON_DEBOUNCE_TIMER_ID],
        .long_press_timer = &controller.soft_timers[PS_LATCH_BUTTON_LONG_PRESS_TIMER_ID],
        .debounce_duration_ms = DEBOUNCE_DURATION_MS,
        .long_press_duration_ms = LONG_PRESS_DURATION_MS,
        .short_press_callback = latch_button_short_press_callback,
        .long_press_callback = NULL
    };
    if (button_init(&interface->latch_button, &latch_button_config) != STATUS_OK) {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

error_t button_interface_update(button_interface_t* interface) {
    
    LED_indicator_set_binary(&interface->phase_shifter_state_indicator, controller.phase_shifter_manager.phase_shifter_buffer[interface->selected_phase_shifter]);
    
    LED_indicator_reset_all(&interface->phase_shifter_select_indicator);
    LED_indicator_set_state(&interface->phase_shifter_select_indicator, interface->selected_phase_shifter, 1);

    LED_indicator_update(&interface->phase_shifter_select_indicator);
    LED_indicator_update(&interface->phase_shifter_state_indicator);

    button_update(&interface->phase_shifter_select_button);
    button_update(&interface->phase_shifter_state_button);
    button_update(&interface->latch_button);
    return STATUS_OK;
}

static button_t* get_button_by_pin(uint16_t GPIO_Pin) {
    
    switch (GPIO_Pin) {
        case PS_SELECT_Pin:
            return &controller.button_interface.phase_shifter_select_button;
        case PS_PHASE_Pin:
            return &controller.button_interface.phase_shifter_state_button;
        case PS_LATCH_Pin:
            return &controller.button_interface.latch_button;
        default:
            return NULL;
    }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {

    button_t *button = get_button_by_pin(GPIO_Pin);
    if (button == NULL) {
        return;
    }

    if (!button->is_debouncing) {

        button->is_debouncing = 1;
        soft_timer_start(button->debounce_timer);
    }

    soft_timer_stop(button->long_press_timer);
    soft_timer_reset(button->long_press_timer);

}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {

    button_t *button = get_button_by_pin(GPIO_Pin);
    if (button == NULL) {
        return;
    }

    if (!button->is_debouncing) {
        button->is_debouncing = 1;
        button->current_event = BUTTON_EVENT_SHORT_PRESS;
        soft_timer_start(button->debounce_timer);
        soft_timer_start(button->long_press_timer);
    }

}
