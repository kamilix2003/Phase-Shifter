
#include "soft_timer.h"

#include <string.h>

void soft_timer_init(soft_timer_t* timer, soft_timer_config_t* config) {
    if (timer == NULL || config == NULL) {
        return;
    }

    timer->callback = config->callback;
    timer->context = config->context;

    timer->period_ms = config->period_ms;
    timer->elapsed_ms = 0;
    timer->is_periodic = config->is_periodic;
    timer->auto_run = config->auto_run;
    timer->ready_to_run = 0;
    timer->is_active = 0;
}

void soft_timer_start(soft_timer_t* timer) {
    if (timer == NULL) {
        return;
    }

    timer->elapsed_ms = 0;
    timer->ready_to_run = 0;
    timer->is_active = 1;
}

void soft_timer_stop(soft_timer_t* timer) {
    if (timer == NULL) {
        return;
    }

    timer->is_active = 0;
    timer->ready_to_run = 0;
}

void soft_timer_reset(soft_timer_t* timer) {
    if (timer == NULL) {
        return;
    }

    timer->elapsed_ms = 0;
    timer->ready_to_run = 0;
}

void soft_timer_execute(soft_timer_t* timer) {
    if (timer == NULL || !timer->is_active || !timer->ready_to_run) {
        return;
    }

    if (timer->callback) {
        timer->callback(timer->context);
    }

    if (timer->is_periodic) {
        timer->elapsed_ms = 0;
        timer->ready_to_run = 0;
    } else {
        timer->is_active = 0;
    }
}

void soft_timer_increment(soft_timer_t* timer, uint32_t delta_ms) {
    if (timer == NULL || !timer->is_active || timer->ready_to_run) {
        return;
    }

    timer->elapsed_ms += delta_ms;

    if (timer->elapsed_ms >= timer->period_ms) {
        timer->ready_to_run = 1;
    }
}

void soft_timer_update(soft_timer_t* timer) {
    if (timer == NULL || !timer->is_active || !timer->ready_to_run) {
        return;
    }
    if (timer->auto_run) {
        soft_timer_execute(timer);
    }
}

void soft_timer_increment_all(soft_timer_t* timers, uint8_t timer_count, uint32_t delta_ms) {
    if (timers == NULL) {
        return;
    }

    for (uint8_t i = 0; i < timer_count; i++) {
        soft_timer_increment(&timers[i], delta_ms);
    }
}

void soft_timer_update_all(soft_timer_t* timers, uint8_t timer_count) {
    if (timers == NULL) {
        return;
    }

    for (uint8_t i = 0; i < timer_count; i++) {
        soft_timer_update(&timers[i]);
    }
}
