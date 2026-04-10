
#include <string.h>

#include "phase_shifter/phase_shifter_manager.h"

#include "phase_shifter_manager.h"
#include "phase_shifter_operations.h"
#include "util/circular_buffer.h"

error_t phase_shifter_manager_init(phase_shifter_manager_t* manager, SPI_HandleTypeDef* hspi) {
    if (manager == NULL || hspi == NULL) {
        return STATUS_ERROR;
    }
    
    manager->hspi = hspi;

    memset(manager->phase_shifter_buffer, 0, PHASE_SHIFTER_COUNT);

    manager->latch_state = 0;

    circular_buffer_init(&manager->operation_queue);
    
    memset(manager->operation_callbacks, 0, sizeof(manager->operation_callbacks));
    
    phase_shifter_manager_register_callback(manager, PHASE_SHIFTER_SET_PHASE_CODE, phase_shifter_set_phase, NULL);
    phase_shifter_manager_register_callback(manager, PHASE_SHIFTER_TRANSMIT_CODE, phase_shifter_transmit, NULL);
    phase_shifter_manager_register_callback(manager, PHASE_SHIFTER_SET_LATCH_CODE, phase_shifter_set_latch, NULL);

    return STATUS_OK;
}

error_t phase_shifter_manager_register_callback(phase_shifter_manager_t* manager, uint8_t operation_code, operation_callback_t callback, void* context) {
    if (manager == NULL || callback == NULL) {
        return STATUS_ERROR;
    }
    
    if (!IS_VALID_OPERATION_CODE(operation_code)) {
        return STATUS_ERROR;
    }
    
    manager->operation_callbacks[operation_code].callback = callback;
    manager->operation_callbacks[operation_code].context = context;
    
    return STATUS_OK;
}

error_t phase_shifter_manager_append_operation(phase_shifter_manager_t* manager, uint8_t operation_code, void* context) {
    if (manager == NULL) {
        return STATUS_ERROR;
    }
    
    if (!IS_VALID_OPERATION_CODE(operation_code)) {
        return STATUS_ERROR;
    }
    
    if (circular_buffer_is_full(&manager->operation_queue)) {
        return STATUS_ERROR;
    }
    
    if (get_phase_shifter_operation_context_size(operation_code) > MAX_MSG_SIZE - 2) {
        return STATUS_ERROR;
    }

    message_t msg = {
        .type = operation_code,
        .length = get_phase_shifter_operation_context_size(operation_code)
    };
    
    memcpy(msg.data, context, msg.length);

    circular_buffer_push(&manager->operation_queue, msg);
    
    return STATUS_OK;
}

error_t phase_shifter_manager_update(phase_shifter_manager_t* manager) {
    if (manager == NULL) {
        return STATUS_ERROR;
    }
    
    if (!circular_buffer_is_empty(&manager->operation_queue)) {
        message_t msg;
        circular_buffer_pop(&manager->operation_queue, &msg);
        
        uint8_t operation_code = msg.type;
        size_t context_size = msg.length;
        void* context = (void*)msg.data;
        
        if (!IS_VALID_OPERATION_CODE(operation_code)) {
            return STATUS_ERROR; // Invalid operation code, skip
        }
        
        operation_callback_t callback = manager->operation_callbacks[operation_code].callback;
        
        if (context_size == 0) {
            context = manager->operation_callbacks[operation_code].context;
        }
        
        if (callback != NULL) {
            callback(manager, context);
        }
    }
    
    return STATUS_OK;
}