
#pragma once 

#include <io.h>
#include <time.h>

#define CHAR_ENTER 0x0D
#define CHAR_VISIBLE_START 0x20
#define CHAR_VISIBLE_END 0x7E
#define CHAR_QUIT 'q'

/*
 * Terminal Controller: controller for displaying user interface
 *
 * This contain logics for:
 * 1. Location of user interface elements
 */
typedef struct {
    RingBuffer commandBuffer;
} TerminalController;

void tc_init(TerminalController *controller);

int tc_process_terminal_input(TerminalController *controller, BufferedChannel *channel);
int tc_process_time(TerminalController *controller, Clock *clock, BufferedChannel *channel);

// Following functions for formatting and positioning
int tc_update_time(TerminalController *controller, long time_ms, BufferedChannel *channel);
int tc_update_sensor(TerminalController *controller, BufferedChannel *channel);

