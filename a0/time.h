/*
 * time.h
 */


typedef struct {
    long time_ms;
    int *value_addr;
    int previous_value;
} Clock;


void cl_init(Clock *clock);

void cl_tick(Clock *clock);

long cl_get_time_ms(Clock *clock);
