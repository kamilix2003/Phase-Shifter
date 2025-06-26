#include "phase_shifter.h"
#include "main.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_spi.h"
#include "stm32g0xx_hal_tim.h"
#include <stdio.h>

static phase_shifter* g_ps;

phase_shifter_status phase_shifter_init(phase_shifter *ps,
    SPI_HandleTypeDef *hspi,
    DMA_HandleTypeDef *hdma,
    TIM_HandleTypeDef *htim) {
    
    ps->hspi = hspi;
    ps->hdma = hdma;
    ps->htim = htim;

    g_ps = ps;

    ps->latch_delay = LATCH_DELAY;
    ps->latch_width = LATCH_WIDTH;
    
    // Initialize buffer
    for (size_t i = 0; i < BUFFER_SIZE; i++) {
        ps->buffer[i] = 0;
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_latch_init(phase_shifter *ps) {
        
    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_set_buffer(phase_shifter *ps, uint8_t *data, size_t size) {
    if (size > BUFFER_SIZE) {
        return PHASE_SHIFTER_ERROR;
    }

    for (size_t i = 0; i < size; i++) {
        ps->buffer[i] = data[i];
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_get_buffer(phase_shifter *ps, uint8_t *data, size_t size) {
    if (size > BUFFER_SIZE) {
        return PHASE_SHIFTER_ERROR;
    }

    for (size_t i = 0; i < size; i++) {
        data[i] = ps->buffer[i];
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_clear_buffer(phase_shifter *ps) {
    for (size_t i = 0; i < BUFFER_SIZE; i++) {
        ps->buffer[i] = 0;
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_set_value(phase_shifter *ps, uint8_t value, size_t index) {
    if (index >= BUFFER_SIZE || value > PHASE_SHIFTER_MAX_VALUE) {
        return PHASE_SHIFTER_ERROR;
    }

    ps->buffer[index] = value;

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_get_value(phase_shifter *ps, uint8_t *value, size_t index) {
    if (index >= BUFFER_SIZE) {
        return PHASE_SHIFTER_ERROR;
    }

    *value = ps->buffer[index];

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_send(phase_shifter *ps) {

    // Check SPI state
    HAL_SPI_StateTypeDef state = HAL_SPI_GetState(ps->hspi);
    if (state != HAL_SPI_STATE_READY) {
        printf("SPI not ready, state: %d\n", state);
        return PHASE_SHIFTER_BUSY;
    }
    
    // Transmit data
    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(ps->hspi, ps->buffer, BUFFER_SIZE);
    if (status != HAL_OK) {
        printf("SPI transmit failed with status: %d\n", status);
        return PHASE_SHIFTER_ERROR;
    }

    return PHASE_SHIFTER_OK;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    // This function is called when the SPI transmission is complete
    if (hspi->Instance == SPI1) {
        // Handle the completion of the SPI transmission if needed
        // For example, you can toggle an LED or send a notification
        // HAL_GPIO_WritePin(PS_LE_GPIO_Port, PS_LE_Pin, 1);

        printf("SPI complete callback\n");

        // HAL_TIM_Base_Start_IT(g_ps->htim); // Start the timer to trigger the latch
        HAL_TIM_OC_Start_IT(g_ps->htim, TIM_CHANNEL_1);
        
    }
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    // This function is called when the timer triggers
    if (htim->Instance == TIM1) {
        // Handle the timer trigger event
        printf("Timer Trigger Callback\n");
        
        // Optionally, you can start the output compare channel here
        HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    // This function is called when the timer period elapses
    if (htim->Instance == TIM1) {
        // Stop the timer to prevent further callbacks
        // HAL_TIM_Base_Stop_IT(htim);
        HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_1); // Start the output compare channel if needed
        
        printf("Timer Period Elapsed Callback\n");
        
        HAL_GPIO_TogglePin(PS_LE_GPIO_Port, PS_LE_Pin); // Toggle the latch pin
    }
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim) {
    // This function is called when the timer triggers
    if (htim->Instance == TIM1) {
        printf("Timer Triggered\n");
    }
}

phase_shifter_status phase_shifter_unlatch(phase_shifter *ps) {
    
    if ( HAL_SPI_GetState(ps->hspi) != HAL_SPI_STATE_READY ) {
        return PHASE_SHIFTER_BUSY;
    }

    HAL_GPIO_WritePin(PS_LE_GPIO_Port, PS_LE_Pin, 1);
    HAL_Delay(100); // Delay to ensure the latch is set
    HAL_GPIO_WritePin(PS_LE_GPIO_Port, PS_LE_Pin, 0);

    return PHASE_SHIFTER_OK;
}
