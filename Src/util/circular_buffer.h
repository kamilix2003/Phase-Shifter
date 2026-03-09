#pragma once

#include <stdint.h>

#define MAX_CIRCULAR_BUFFER_SIZE 64
#define MAX_MSG_SIZE 64

typedef enum {
    BUFFER_OK = 0,
    BUFFER_ERROR,
    BUFFER_FULL,
    BUFFER_EMPTY
} buffer_status_t;

typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t data[MAX_MSG_SIZE - 2];
} message_t;

typedef struct {
    uint8_t buffer[MAX_CIRCULAR_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} circular_buffer_t;

buffer_status_t circular_buffer_init(circular_buffer_t* cb);
buffer_status_t circular_buffer_push(circular_buffer_t* cb, message_t msg);
buffer_status_t circular_buffer_pop(circular_buffer_t* cb, message_t* msg);
buffer_status_t circular_buffer_peek(circular_buffer_t* cb, message_t* msg);
buffer_status_t circular_buffer_is_full(circular_buffer_t* cb);
buffer_status_t circular_buffer_is_empty(circular_buffer_t* cb);