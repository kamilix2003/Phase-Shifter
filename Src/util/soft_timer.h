
#define MAX_SOFT_TIMER_COUNT 8

typedef void (*soft_timer_callback_t)(void* context);

typedef struct {
    soft_timer_callback_t callback;
    void* context;
    
    uint32_t period_ms;
    uint32_t elapsed_ms;

    uint8_t is_periodic;
    uint8_t run_immediately;
    uint8_t is_active;
} soft_timer_t;

soft_timer_t soft_timers[MAX_SOFT_TIMER_COUNT];

void soft_timer_init(soft_timer_t* timer, soft_timer_callback_t callback, void* context, uint32_t period_ms, uint8_t is_periodic);
void soft_timer_start(soft_timer_t* timer);
void soft_timer_stop(soft_timer_t* timer);
void soft_timer_reset(soft_timer_t* timer);
void soft_timer_execute(soft_timer_t* timer);
void soft_timer_update(soft_timer_t* timer, uint32_t delta_ms);

