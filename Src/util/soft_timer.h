#pragma once

#include <stdint.h>

typedef void (*soft_timer_callback_t)(void* context);

typedef struct {
    soft_timer_callback_t callback;
    void* context;
    
    uint32_t period_ms;     // Timer period in milliseconds
    uint32_t elapsed_ms;    // Elapsed time in milliseconds

    uint8_t is_periodic;    // 1 for periodic timers, 0 for one-shot timers
    uint8_t ready_to_run;   // Flag to indicate if the timer is ready to execute
    uint8_t auto_run;       // Flag to indicate if the timer should automatically restart after execution
    uint8_t is_active;      // Flag to indicate if the timer is currently active
} soft_timer_t;

typedef struct {
    soft_timer_callback_t callback;
    void* context;
    
    uint32_t period_ms;     

    uint8_t is_periodic;    
    uint8_t auto_run;       
} soft_timer_config_t;

void soft_timer_init(soft_timer_t* timer, soft_timer_config_t* config);
void soft_timer_start(soft_timer_t* timer);
void soft_timer_stop(soft_timer_t* timer);
void soft_timer_reset(soft_timer_t* timer);
void soft_timer_execute(soft_timer_t* timer);
void soft_timer_increment(soft_timer_t* timer, uint32_t delta_ms);
void soft_timer_update(soft_timer_t* timer);

void soft_timer_increment_all(soft_timer_t** timers, uint8_t timer_count, uint32_t delta_ms);
void soft_timer_update_all(soft_timer_t** timers, uint8_t timer_count);

