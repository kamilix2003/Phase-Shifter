
#include "phase_shifter.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_spi.h"
#include "../phase_shifter/phase_shifter.h"
#include "../beam_controller.h"

extern beam_controller_TypeDef beam_controller;

phase_shifter_StatusTypeDef phase_shifter_init(phase_shifter_TypeDef *hps, SPI_HandleTypeDef *hspi, GPIO_TypeDef *latch_port, uint16_t latch_pin){
    if(hps == NULL || hspi == NULL || latch_port == NULL) return PHASE_SHIFTER_ERROR;

    hps->hspi = hspi;
    hps->latch_port = latch_port;
    hps->latch_pin = latch_pin;

    HAL_GPIO_WritePin(hps->latch_port, hps->latch_pin, GPIO_PIN_RESET);

    hps->auto_latch_enabled = 1;

    for(size_t i = 0; i < PHASE_SHIFTER_COUNT; i++){
        hps->buffer[i] = 0;
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_StatusTypeDef phase_shifter_deinit(phase_shifter_TypeDef *hps){
    if(hps == NULL) return PHASE_SHIFTER_ERROR;

    hps->hspi = NULL;
    hps->latch_port = NULL;
    hps->latch_pin = 0;

    for(size_t i = 0; i < PHASE_SHIFTER_COUNT; i++){
        hps->buffer[i] = 0;
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_StatusTypeDef phase_shifter_set(phase_shifter_TypeDef *hps, uint8_t *data, size_t size){
    if(hps == NULL || data == NULL || size != PHASE_SHIFTER_COUNT) return PHASE_SHIFTER_ERROR;
    if(hps->hspi == NULL) return PHASE_SHIFTER_ERROR;

    while(hps->hspi->State == HAL_SPI_STATE_BUSY);

    for(size_t i = 0; i < size; i++){
        // if(data[i] >= (1 << PHASE_SHIFTER_BITS)) return PHASE_SHIFTER_ERROR;
        hps->buffer[i] = data[i] << 2;
    }

    if(HAL_SPI_Transmit_IT(hps->hspi, hps->buffer, PHASE_SHIFTER_COUNT) != HAL_OK){
        return PHASE_SHIFTER_ERROR;
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_StatusTypeDef phase_shifter_latch(phase_shifter_TypeDef *hps, uint8_t state){
    if(hps == NULL) return PHASE_SHIFTER_ERROR;
    if(hps->latch_port == NULL || hps->latch_pin == 0) return PHASE_SHIFTER_ERROR;

    if(state){
        HAL_GPIO_WritePin(hps->latch_port, hps->latch_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(hps->latch_port, hps->latch_pin, GPIO_PIN_RESET);
    }

    return PHASE_SHIFTER_OK;
}

phase_shifter_StatusTypeDef phase_shifter_unlatch(phase_shifter_TypeDef *hps){
    if(hps == NULL) return PHASE_SHIFTER_ERROR;
    if(hps->latch_port == NULL || hps->latch_pin == 0) return PHASE_SHIFTER_ERROR;

    HAL_GPIO_WritePin(hps->latch_port, hps->latch_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hps->latch_port, hps->latch_pin, GPIO_PIN_RESET);

    return PHASE_SHIFTER_OK;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
    if(hspi == NULL) return;
    
    if(hspi == beam_controller.phase_shifter.hspi){
        if (beam_controller.phase_shifter.auto_latch_enabled) {
            // HAL_Delay(1);
            phase_shifter_unlatch(&beam_controller.phase_shifter);
        }
    }
}
