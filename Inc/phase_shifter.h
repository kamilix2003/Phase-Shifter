
#ifndef PHASE_SHIFTER_H
#define PHASE_SHIFTER_H

#include <stdint.h>
#include <stddef.h>
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"

#define BUFFER_SIZE 4
#define PHASE_SHIFTER_MAX_VALUE 15

#define LATCH_DELAY 0xFF // Delay in microseconds for latching
#define LATCH_WIDTH 0xF // Width of the latch pulse in microseconds

typedef enum {
    PHASE_SHIFTER_OK = 0,
    PHASE_SHIFTER_ERROR = -1,
    PHASE_SHIFTER_BUSY = -2,
    PHASE_SHIFTER_TIMEOUT = -3
} phase_shifter_status;

typedef struct {

    SPI_HandleTypeDef * hspi; // SPI handle for communication
    DMA_HandleTypeDef * hdma; // DMA handle for SPI transmission
    TIM_HandleTypeDef * htim; // Timer handle for synchronization

    uint32_t latch_delay; // Delay for latching the data
    uint32_t latch_width; // Width of the latch pulse

    uint8_t tx_ready;

    uint8_t auto_latch; // Flag to indicate if auto-latching is enabled

    uint8_t buffer[BUFFER_SIZE];

} phase_shifter;

phase_shifter_status phase_shifter_init(phase_shifter *ps,
    SPI_HandleTypeDef *hspi,
    DMA_HandleTypeDef *hdma,
    TIM_HandleTypeDef *htim);

phase_shifter_status phase_shifter_latch_init(phase_shifter *ps);

phase_shifter_status phase_shifter_set_buffer(phase_shifter *ps, uint8_t *data, size_t size);
phase_shifter_status phase_shifter_get_buffer(phase_shifter *ps, uint8_t *data, size_t size);
phase_shifter_status phase_shifter_clear_buffer(phase_shifter *ps);

phase_shifter_status phase_shifter_set_value(phase_shifter *ps, uint8_t value, size_t index);
phase_shifter_status phase_shifter_get_value(phase_shifter *ps, uint8_t *value, size_t index);

phase_shifter_status phase_shifter_send(phase_shifter *ps);
phase_shifter_status phase_shifter_unlatch(phase_shifter *ps);

#endif // PHASE_SHIFTER_H
