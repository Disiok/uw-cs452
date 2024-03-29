#include "time.h"
#include <ts7200.h>

// We use slow clock speed at 2k Hz.
#define CLOCK_SPEED 2000

// We want a period to be 100 ms
#define PERIOD_TIME_MS 100

// Thus we want a period with 20 ticks
#define PERIOD_TICK CLOCK_SPEED / 1000 * PERIOD_TIME_MS

#define DISPLAY_UPDATE_TIME_MS 100

void cl_init(Clock *clock) {
    // load starting value into load register for clock
    int *clock_load_addr = (int *) (TIMER3_BASE + LDR_OFFSET);
    *clock_load_addr = PERIOD_TICK;

    // flip enable bit to start clock, set to periodic mode, use slower clock
    int *clock_control_addr = (int *) (TIMER3_BASE + CRTL_OFFSET);
    *clock_control_addr = (ENABLE_MASK | MODE_MASK) & ~CLKSEL_MASK;

    clock->value_addr = (int *) (TIMER3_BASE + VAL_OFFSET);
    clock->time_ms = 0;
    clock->base_time_ms = 0;
    clock->previous_value = PERIOD_TICK;
    clock->time_changed = 1;
}

void cl_poll(Clock *clock, SmartTerminal *st) {
    int clock_value = *(clock->value_addr);
    if (clock_value > clock->previous_value) {
        clock->base_time_ms += PERIOD_TIME_MS;

        if (clock->base_time_ms % DISPLAY_UPDATE_TIME_MS == 0) {
            clock->time_changed = 1;
        }
    }
    clock->time_ms = clock->base_time_ms + (PERIOD_TIME_MS - clock_value / 2);
    clock->previous_value = clock_value;

    if (clock->time_changed) {
        st_update_time(st, clock->time_ms);
    }
}

int cl_get_time_ms(Clock *clock) {
    clock->time_changed = 0;
    return clock->time_ms;
}

int cl_time_changed(Clock *clock) {
    return clock->time_changed;
}
