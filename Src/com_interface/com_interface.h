#pragma once

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include <sys/types.h>

#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 64

#define COMMAND_BUFFER_SIZE 64

typedef struct {
    uint8_t command;
    uint8_t data_length;
    uint8_t data[COMMAND_BUFFER_SIZE - 2];
} com_interface_message_t;

typedef struct {

    // uint8_t text_mode;

    com_interface_message_t rx_buffer[RX_BUFFER_SIZE];
    volatile size_t rx_head;
    volatile size_t rx_tail;

    com_interface_message_t tx_buffer[TX_BUFFER_SIZE];
    volatile size_t tx_head;
    volatile size_t tx_tail;

} com_interface_TypeDef;

typedef enum {
    COM_INTERFACE_OK = 0,
    COM_INTERFACE_ERROR = -1,
    COM_INTERFACE_BUSY = -2,
    COM_INTERFACE_TIMEOUT = -3
}com_interface_StatusTypeDef;

com_interface_StatusTypeDef com_interface_init(com_interface_TypeDef* com_interface);
com_interface_StatusTypeDef com_interface_deinit(com_interface_TypeDef* com_interface);

com_interface_StatusTypeDef com_interface_add_tx(com_interface_TypeDef* com_interface, com_interface_message_t* msg);
com_interface_StatusTypeDef com_interface_add_rx(com_interface_TypeDef* com_interface, uint8_t* buf, uint32_t len);
com_interface_StatusTypeDef com_interface_process_rx(com_interface_TypeDef* com_interface);
com_interface_StatusTypeDef com_interface_process_tx(com_interface_TypeDef* com_interface);
