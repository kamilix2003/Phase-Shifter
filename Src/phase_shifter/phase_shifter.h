
#include "../../Core/Inc/spi.h"
#include "../../Core/Inc/dma.h"
#include "stm32g0b1xx.h"

#define PHASE_SHIFTER_COUNT 4
#define PHASE_SHIFTER_BITS 4

typedef struct{
    SPI_HandleTypeDef *hspi;
    DMA_HandleTypeDef *hdma;

    uint8_t buffer[PHASE_SHIFTER_COUNT];
}spi_phase_shifter_TypeDef;

typedef enum {
    PHASE_SHIFTER_OK = 0,
    PHASE_SHIFTER_ERROR = -1,
    PHASE_SHIFTER_BUSY = -2,
    PHASE_SHIFTER_TIMEOUT = -3
}phase_shifter_StatusTypeDef;

phase_shifter_StatusTypeDef spi_phase_shifter_init(spi_phase_shifter_TypeDef *hps, SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma);
phase_shifter_StatusTypeDef spi_phase_shifter_deinit(spi_phase_shifter_TypeDef *hps);
phase_shifter_StatusTypeDef spi_phase_shifter_set(spi_phase_shifter_TypeDef *hps, uint8_t *data, size_t size);
phase_shifter_StatusTypeDef spi_phase_shifter_clear(spi_phase_shifter_TypeDef *hps);
phase_shifter_StatusTypeDef spi_phase_shifter_unlatch(spi_phase_shifter_TypeDef *hps);

