
#include "beam_controller.h"
#include "com_interface/com_interface.h"
#include "main.h"
#include "stm32g0xx_hal_uart.h"

beam_controller_StatusTypeDef beam_controller_init(beam_controller_TypeDef* controller, SPI_HandleTypeDef* hspi, GPIO_TypeDef* latch_port, uint16_t latch_pin) {
    if (controller == NULL) {
        return BEAM_CONTROLLER_ERROR;
    }

    if (com_interface_init(&controller->com_interface) != COM_INTERFACE_OK) {
        return BEAM_CONTROLLER_ERROR;
    }

    if (phase_shifter_init(&controller->phase_shifter, hspi, latch_port, latch_pin) != PHASE_SHIFTER_OK) {
        com_interface_deinit(&controller->com_interface);
        return BEAM_CONTROLLER_ERROR;
    }

    return BEAM_CONTROLLER_OK;
}

beam_controller_StatusTypeDef beam_controller_deinit(beam_controller_TypeDef* controller) {
    if (controller == NULL) {
        return BEAM_CONTROLLER_ERROR;
    }

    if (com_interface_deinit(&controller->com_interface) != COM_INTERFACE_OK) {
        return BEAM_CONTROLLER_ERROR;
    }

    if (phase_shifter_deinit(&controller->phase_shifter) != PHASE_SHIFTER_OK) {
        return BEAM_CONTROLLER_ERROR;
    }

    return BEAM_CONTROLLER_OK;
}

beam_controller_StatusTypeDef beam_controller_update(beam_controller_TypeDef *controller) {
    if (controller == NULL) {
        return BEAM_CONTROLLER_ERROR;
    }

    if (com_interface_process_rx(&controller->com_interface) == COM_INTERFACE_ERROR) {
        return BEAM_CONTROLLER_ERROR;
    }
    if (com_interface_process_tx(&controller->com_interface) == COM_INTERFACE_ERROR) {
        return BEAM_CONTROLLER_ERROR;
    }

    return BEAM_CONTROLLER_OK;
}
