#pragma once


#include "util/circular_buffer.h"
#include "util/error_handle.h"

#include "com_interface/commands.h"

typedef struct {
    circular_buffer_t rx_buffer;
    circular_buffer_t tx_buffer;
} com_interface_t;

error_t com_interface_init(com_interface_t* com_interface);

error_t com_interface_process_rx(com_interface_t* com_interface);
error_t com_interface_process_tx(com_interface_t* com_interface);

error_t com_interface_add_tx_message(com_interface_t* com_interface, message_t msg);
