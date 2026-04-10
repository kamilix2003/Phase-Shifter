
#include "led_indicator.h"
#include "util/error_handle.h"
#include <string.h>

error_t LED_indicator_init(LED_indicator* indicator, GPIO_TypeDef* LED_ports[LED_COUNT], uint32_t LED_pins[LED_COUNT]) {
    
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_port[i] = LED_ports[i];
        indicator->LED_pins[i] = LED_pins[i];
        indicator->LED_states[i] = 0;
        HAL_GPIO_WritePin(indicator->LED_port[i], indicator->LED_pins[i], GPIO_PIN_RESET);
    }
    return STATUS_OK;
}

error_t LED_indicator_update(LED_indicator* indicator) {
    for (int i = 0; i < LED_COUNT; i++) {
        HAL_GPIO_WritePin(indicator->LED_port[i], indicator->LED_pins[i], indicator->LED_states[i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    return STATUS_OK;
}

error_t LED_indicator_set_state(LED_indicator* indicator, uint8_t led_index, uint8_t state) {
    if (led_index >= LED_COUNT) {
        return STATUS_ERROR;
    }
    indicator->LED_states[led_index] = state ? 1 : 0;
    return STATUS_OK;
}

error_t LED_indicator_set_state_all(LED_indicator* indicator, uint8_t state[LED_COUNT]) {
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_states[i] = state[i] ? 1 : 0;
    }
    return STATUS_OK;
}

error_t LED_indicator_toggle_state(LED_indicator* indicator, uint8_t led_index) {
    if (led_index >= LED_COUNT) {
        return STATUS_ERROR;
    }
    indicator->LED_states[led_index] ^= 1;
    return STATUS_OK;
}

error_t LED_indicator_toggle_all(LED_indicator* indicator) {
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_states[i] ^= 1;
    }
    return STATUS_OK;
}

error_t LED_indicator_reset_all(LED_indicator* indicator) {
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_states[i] = 0;
    }
    return STATUS_OK;
}

error_t LED_indicator_set_all(LED_indicator* indicator) {
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_states[i] = 1;
    }
    return STATUS_OK;
}

error_t LED_indicator_set_binary(LED_indicator* indicator, uint8_t value) {
    for (int i = 0; i < LED_COUNT; i++) {
        indicator->LED_states[i] = (value >> i) & 0x01;
    }
    return STATUS_OK;
}