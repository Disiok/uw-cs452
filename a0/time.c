#include "time.h"
#include <ts7200.h>

// We use slow clock speed at 2k Hz.
#define CLOCK_SPEED 2000

// We want a period to be 1 ms
#define PERIOD_TIME_MS 1

// Thus we want a period with 2 ticks
#define PERIOD_TICK CLOCK_SPEED / (PERIOD_TIME_MS * 1000)

void cl_init(Clock *clock) {
    // load starting value into load register for clock
    int *clock_load_addr = (int *) (TIMER3_BASE + LDR_OFFSET);
    *clock_load_addr = PERIOD_TICK;

    // flip enable bit to start clock, set to periodic mode, use slower clock
    int *clock_control_addr = (int *) (TIMER3_BASE + CRTL_OFFSET);
    *clock_control_addr = ENABLE_MASK | MODE_MASK & ~CLKSEL_MASK;

    clock->value_addr = (int *) (TIMER3_BASE + VAL_OFFSET);
    clock->time_ms = 0;
    clock->previous_value = PERIOD_TICK;
}

void cl_tick(Clock *clock) {
    int clock_value = *(clock->value_addr);
    if (clock_value > clock->previous_value) {
        clock->time_ms ++;
    }
    clock->previous_value = clock_value;
}

long cl_get_time_ms(Clock *clock) {
    return clock->time_ms;
}