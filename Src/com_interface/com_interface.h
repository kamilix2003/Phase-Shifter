#pragma once


#include "util/circular_buffer.h"
#include "util/error_handle.h"

#include "com_interface/commands.h"

#define COM_COMMAND_COUNT 32

typedef void (*com_command_handler_t)(const message_t* msg, void* context);

typedef struct {
    com_command_handler_t handler;
    void* context;
} com_command_t;

#define ENDPOINT_COUNT 1

typedef void (*tx_endpoint_t)(uint8_t* data, uint32_t length);

typedef struct {

    circular_buffer_t rx_buffer;
    circular_buffer_t tx_buffer;

    tx_endpoint_t tx_endpoints[ENDPOINT_COUNT];

    com_command_t commands[COM_COMMAND_COUNT];

} com_interface_t;

error_t com_interface_init(com_interface_t* com_interface);
error_t com_interface_update(com_interface_t* com_interface);

error_t com_interface_register_command(com_interface_t* com_interface, uint8_t command_id, com_command_handler_t handler, void* context);

error_t com_interface_process_rx(com_interface_t* com_interface);
error_t com_interface_process_tx(com_interface_t* com_interface);

error_t com_interface_add_rx_message(com_interface_t* com_interface, message_t msg);
error_t com_interface_add_tx_message(com_interface_t* com_interface, message_t msg);
