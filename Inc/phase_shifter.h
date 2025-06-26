
#ifndef PHASE_SHIFTER_H
#define PHASE_SHIFTER_H

#include <stdint.h>
#include <stddef.h>
#include "stm32g0xx_hal.h"

#define BUFFER_SIZE 4
#define PHASE_SHIFTER_MAX_VALUE 31

typedef enum {
    PHASE_SHIFTER_OK = 0,
    PHASE_SHIFTER_ERROR = -1,
    PHASE_SHIFTER_BUSY = -2,
    PHASE_SHIFTER_TIMEOUT = -3
} phase_shifter_status;

typedef struct {

    SPI_HandleTypeDef hspi; // SPI handle for communication
    DMA_HandleTypeDef hdma; // DMA handle for SPI transmission

    uint8_t buffer[BUFFER_SIZE];

} phase_shifter;

phase_shifter_status phase_shifter_init(phase_shifter *ps,
    SPI_HandleTypeDef *hspi,
    DMA_HandleTypeDef *hdma);

phase_shifter_status phase_shifter_set_buffer(phase_shifter *ps, uint8_t *data, size_t size);
phase_shifter_status phase_shifter_get_buffer(phase_shifter *ps, uint8_t *data, size_t size);
phase_shifter_status phase_shifter_clear_buffer(phase_shifter *ps);

phase_shifter_status phase_shifter_set_value(phase_shifter *ps, uint8_t value, size_t index);
phase_shifter_status phase_shifter_get_value(phase_shifter *ps, uint8_t *value, size_t index);

phase_shifter_status phase_shifter_send(phase_shifter *ps, uint8_t unlatch);
phase_shifter_status phase_shifter_unlatch(phase_shifter *ps);

#endif // PHASE_SHIFTER_H
