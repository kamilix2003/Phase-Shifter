
#include "beam_controller.h"

error_t beam_controller_init(beam_controller_t* controller, const beam_controller_config_t* config) {
    error_t err;

    // Initialize the phase shifter manager
    err = phase_shifter_manager_init(&controller->phase_shifter_manager, config->hspi);
    if (err != STATUS_OK) {
        return err;
    }

    return STATUS_OK;
}