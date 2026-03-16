
#include "button_interface.h"
#include "beam_controller.h"
#include "led_indicator.h"

#include "stm32g0b1xx.h"
#include "stm32g0xx_hal_gpio.h"
#include "util/error_handle.h"

#include "main.h"

error_t button_interface_init(button_interface_t* interface) {
    // Initialize LED indicators
    static const GPIO_TypeDef* select_ports[LED_COUNT] = {LED_PS_SELECT_0_GPIO_Port, LED_PS_SELECT_1_GPIO_Port, LED_PS_SELECT_2_GPIO_Port, LED_PS_SELECT_3_GPIO_Port};
    static const uint32_t select_pins[LED_COUNT] = {LED_PS_SELECT_0_Pin, LED_PS_SELECT_1_Pin, LED_PS_SELECT_2_Pin, LED_PS_SELECT_3_Pin};
    if (LED_indicator_init(&interface->phase_shifter_select_indicator, select_ports, select_pins) != STATUS_OK) {
        return STATUS_ERROR;
    }
    static const GPIO_TypeDef* state_ports[LED_COUNT] = {LED_PS_STATE_0_GPIO_Port, LED_PS_STATE_1_GPIO_Port, LED_PS_STATE_2_GPIO_Port, LED_PS_STATE_3_GPIO_Port};
    static const uint32_t state_pins[LED_COUNT] = {LED_PS_STATE_0_Pin, LED_PS_STATE_1_Pin, LED_PS_STATE_2_Pin, LED_PS_STATE_3_Pin};
    if (LED_indicator_init(&interface->phase_shifter_state_indicator, state_ports, state_pins) != STATUS_OK) {
        return STATUS_ERROR;
    }

    interface->selected_phase_shifter = 0;

     // Initialize buttons

    return STATUS_OK;
}

error_t button_interface_update(button_interface_t* interface) {
    
    LED_indicator_set_binary(&interface->phase_shifter_state_indicator, controller.phase_shifter_manager.phase_shifter_buffer[interface->selected_phase_shifter]);
    
    LED_indicator_reset_all(&interface->phase_shifter_select_indicator);
    LED_indicator_set_state(&interface->phase_shifter_select_indicator, interface->selected_phase_shifter, 1);

    LED_indicator_update(&interface->phase_shifter_select_indicator);
    LED_indicator_update(&interface->phase_shifter_state_indicator);

    return STATUS_OK;
}
