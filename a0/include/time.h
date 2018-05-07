/*
 * time.h
 */

#pragma once

typedef struct {
    long time_ms;
    int *value_addr;
    int previous_value;
    int time_changed;
} Clock;


#include <terminal.h>

void cl_init(Clock *clock);

void cl_poll(Clock *clock, SmartTerminal *st);

long cl_get_time_ms(Clock *clock);

int cl_time_changed(Clock *clock);
