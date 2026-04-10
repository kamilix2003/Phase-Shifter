
#include "circular_buffer.h"
#include <string.h>

buffer_status_t circular_buffer_init(circular_buffer_t* cb) {
	if (cb == NULL) {
		return BUFFER_ERROR;
	}

	cb->head = 0;
	cb->tail = 0;
	cb->count = 0;

	return BUFFER_OK;
}

buffer_status_t circular_buffer_push(circular_buffer_t* cb, message_t msg) {
	if (cb == NULL) {
		return BUFFER_ERROR;
	}

	if (cb->count >= MAX_CIRCULAR_BUFFER_SIZE) {
		return BUFFER_FULL;
	}

	memcpy(&cb->buffer[cb->head], &msg, sizeof(message_t));
	cb->head = (uint8_t)((cb->head + 1U) % MAX_CIRCULAR_BUFFER_SIZE);
	cb->count++;

	return BUFFER_OK;
}

buffer_status_t circular_buffer_pop(circular_buffer_t* cb, message_t* msg) {
	if ((cb == NULL) || (msg == NULL)) {
		return BUFFER_ERROR;
	}

	if (cb->count == 0) {
		return BUFFER_EMPTY;
	}

	*msg = *(message_t*)&cb->buffer[cb->tail];
	cb->tail = (uint8_t)((cb->tail + 1U) % MAX_CIRCULAR_BUFFER_SIZE);
	cb->count--;

	return BUFFER_OK;
}

buffer_status_t circular_buffer_peek(circular_buffer_t* cb, message_t* msg) {
	if ((cb == NULL) || (msg == NULL)) {
		return BUFFER_ERROR;
	}

	if (cb->count == 0) {
		return BUFFER_EMPTY;
	}

	*msg = *(message_t*)&cb->buffer[cb->tail];
	return BUFFER_OK;
}

buffer_status_t circular_buffer_is_full(circular_buffer_t* cb) {
	if (cb == NULL) {
		return BUFFER_ERROR;
	}

	return (cb->count >= MAX_CIRCULAR_BUFFER_SIZE) ? BUFFER_FULL : BUFFER_OK;
}

buffer_status_t circular_buffer_is_empty(circular_buffer_t* cb) {
	if (cb == NULL) {
		return BUFFER_ERROR;
	}

	return (cb->count == 0) ? BUFFER_EMPTY : BUFFER_OK;
}