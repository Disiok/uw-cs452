
#pragma once 

#include <io.h>
#include <train.h>
#include <track_node.h>
#include <time.h>

/*
 * Smart Terminal: wrapper on gtk for moving cursors around
 *
 * This contains logic for:
 * 1. Basic functions dealing with cursors
 */

// Special characters
#define CHAR_ENTER 0x0D
#define CHAR_BACKSPACE 0x08
#define CHAR_VISIBLE_START 0x20
#define CHAR_VISIBLE_END 0x7E
#define CHAR_QUIT 'q'

// Rows
#define STATUS_ROW 30
#define HISTORY_ROW 31
#define PROMPT_ROW 32
#define SWITCHES_ROW_START 3
#define SENSORS_ROW_START 3

// Columns
#define START_COL 0
#define DYNAMIC_COL 10
#define SWITCHES_COL_START 50
#define SENSORS_COL_START 0

#define COMMAND_BUFFER_SIZE 64

typedef struct {
    BufferedChannel channel;
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int size;
} SmartTerminal;

/*
 * Terminal Controller: controller for displaying user interface
 *
 * This contain logics for:
 * 1. Location of user interface elements
 */
typedef struct {
    SmartTerminal *st;
} TerminalController;

void tc_init(TerminalController *controller, SmartTerminal *st);
int tc_process_time(TerminalController *controller, Clock *clock);
// Following functions for formatting and positioning
void tc_render_static(TerminalController *controller, track_node *track);
int tc_update_time(TerminalController *controller, long time_ms);
int tc_update_sensor(TerminalController *controller);

void st_init(SmartTerminal *st);
void st_poll(SmartTerminal *st);
int st_process_terminal_input(SmartTerminal *st, TerminalController *terminal_controller, TrainController *train_controller);
void st_save_cursor(SmartTerminal *st);
void st_restore_cursor(SmartTerminal *st);
void st_move_cursor(SmartTerminal *st, int vertical, int horizontal);
void st_move_cursor_top_left(SmartTerminal *st);
void st_clear_line_from_cursor(SmartTerminal *st);
void st_clear_line(SmartTerminal *st);
void st_clear_screen(SmartTerminal *st);

