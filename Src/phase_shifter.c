
#include "phase_shifter.h"
#include "main.h"
#include <stdio.h>

phase_shifter_status phase_shifter_init(phase_shifter *ps, SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma) {

    ps->hspi = *hspi;
    ps->hdma = *hdma;

    for (size_t i = 0; i < BUFFER_SIZE; i++) {
        ps->buffer[i] = 0;
    }

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

phase_shifter_status phase_shifter_send(phase_shifter *ps, uint8_t unlatch) {

    if (HAL_SPI_GetState(&ps->hspi) != HAL_SPI_STATE_READY) {
        return PHASE_SHIFTER_BUSY;
    }

    if (HAL_SPI_Transmit_DMA(&ps->hspi, ps->buffer, BUFFER_SIZE) != HAL_OK) {
        printf("Error: SPI transmit failed %li \n", HAL_SPI_GetError(&ps->hspi));
        return PHASE_SHIFTER_ERROR;
    }

    if (unlatch) {
        return phase_shifter_unlatch(ps);
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_status phase_shifter_unlatch(phase_shifter *ps) {
    
    if ( HAL_SPI_GetState(&ps->hspi) != HAL_SPI_STATE_READY ) {
        return PHASE_SHIFTER_BUSY;
    }

    HAL_GPIO_WritePin(PS_LE_GPIO_Port, PS_LE_Pin, 1);
    HAL_Delay(1); // Delay to ensure the latch is set
    HAL_GPIO_WritePin(PS_LE_GPIO_Port, PS_LE_Pin, 0);

    return PHASE_SHIFTER_OK;
}
