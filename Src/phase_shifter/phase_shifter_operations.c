#include "config.h"
#include "main.h"
#include "phase_shifter_manager.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_spi.h"
#include <string.h>

#include "phase_shifter/phase_shifter_operations.h"

operation_callback_t phase_shifter_set_phase(phase_shifter_manager_t* manager, void* context) {
    
    phase_shifter_set_phase_context_t* ctx = (phase_shifter_set_phase_context_t*)context;

    memcpy(manager->phase_shifter_buffer, ctx->phase_shifter_buffer, PHASE_SHIFTER_COUNT);

}

static void phase_shifter_resolution_translation(uint8_t* buffer, uint8_t* transmit_buffer){
    memcpy(transmit_buffer, buffer, PHASE_SHIFTER_COUNT);
    for (size_t i = 0; i < PHASE_SHIFTER_COUNT; i++) {
        transmit_buffer[i] = transmit_buffer[i] << (6 - PHASE_SHIFTER_BIT_RESOLUTION);
    }
}

operation_callback_t phase_shifter_transmit(phase_shifter_manager_t* manager, void* context) {
    // No additional context needed for transmit operation

    static uint8_t transmit_buffer[PHASE_SHIFTER_COUNT];

    phase_shifter_resolution_translation(manager->phase_shifter_buffer, transmit_buffer);
    HAL_SPI_Transmit_IT(manager->hspi, transmit_buffer, PHASE_SHIFTER_COUNT);

}

operation_callback_t phase_shifter_set_latch(phase_shifter_manager_t* manager, void* context) {
    
    phase_shifter_set_latch_context_t* ctx = (phase_shifter_set_latch_context_t*)context;

    manager->latch_state = ctx->latch_state;

    HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, manager->latch_state);
    
}

size_t get_phase_shifter_operation_context_size(uint8_t operation_code) {
    if (!IS_VALID_OPERATION_CODE(operation_code)) {
        return 0; // Invalid operation code
    }
    
    return phase_shifter_operation_context_size_map[operation_code];
}