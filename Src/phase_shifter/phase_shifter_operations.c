#include "main.h"
#include "phase_shifter_manager.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_spi.h"
#include <string.h>

#include "phase_shifter/phase_shifter_operations.h"

operation_callback_t phase_shifter_set_phase(phase_shifter_manager_t* manager, void* context) {
    
    phase_shifter_set_phase_context_t* ctx = (phase_shifter_set_phase_context_t*)context;

    memcpy(ctx->phase_shifter_buffer, manager->phase_shifter_buffer, ctx->buffer_length);

}

operation_callback_t phase_shifter_transmit(phase_shifter_manager_t* manager, void* context) {
    // No additional context needed for transmit operation

    HAL_SPI_Transmit_IT(manager->hspi, &manager->phase_shifter_buffer, PHASE_SHIFTER_COUNT);

}

operation_callback_t phase_shifter_set_latch(phase_shifter_manager_t* manager, void* context) {
    
    phase_shifter_set_latch_context_t* ctx = (phase_shifter_set_latch_context_t*)context;

    manager->latch_state = ctx->latch_state;

    HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, manager->latch_state);
    
}