
#pragma once 

typedef struct SmartTerminal SmartTerminal;
typedef struct TerminalController TerminalController;

#include <io.h>
#include <train.h>
#include <time.h>
#include <track.h>

/*
 * Smart Terminal: wrapper on gtk for moving cursors around
 *
 * This contains logic for:
 * 1. Basic functions dealing with cursors
 * 2. Location of user interface elements
 */

// Special characters
#define CHAR_ENTER 0x0D
#define CHAR_BACKSPACE 0x08
#define CHAR_VISIBLE_START 0x20
#define CHAR_VISIBLE_END 0x7E
#define CHAR_QUIT 'q'

// Rows
#define STATUS_ROW 20
#define HISTORY_ROW 21
#define PROMPT_ROW 22
#define SWITCHES_ROW_START 4
#define SENSORS_ROW_START 4

// Columns
#define START_COL 1
#define DYNAMIC_COL 10
#define SWITCHES_COL_START 1
#define SENSORS_COL_START 50

#define COMMAND_BUFFER_SIZE 64

struct SmartTerminal{
    BufferedChannel channel;
};

void st_init(SmartTerminal *st);
void st_poll(SmartTerminal *st);
void st_save_cursor(SmartTerminal *st);
void st_restore_cursor(SmartTerminal *st);
void st_move_cursor(SmartTerminal *st, int vertical, int horizontal);
void st_move_cursor_top_left(SmartTerminal *st);
void st_clear_line_from_cursor(SmartTerminal *st);
void st_clear_line(SmartTerminal *st);
void st_clear_screen(SmartTerminal *st);

// Following functions for formatting and positioning
void st_render_static(SmartTerminal *st, Track *track);
int st_update_status(SmartTerminal *st, char *status);
int st_update_history(SmartTerminal *st, char *history);
int st_update_command(SmartTerminal *st, char *command);
int st_update_time(SmartTerminal *st, int time_ms);
int st_update_sensors(SmartTerminal *st, RingBuffer *sensorBuffer, int sensorFlag);
int st_update_switch(SmartTerminal *st, int switch_numer, char state, Track *track);
int st_update_switch_all(SmartTerminal *st, char state, Track *track);

/*
 * Terminal Controller: controller for displaying user interface
 *
 * This contain logics for:
 * 1. Parsing commands
 */
struct TerminalController {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int size;
};

void tc_init(TerminalController *controller);
int tc_poll(TerminalController *terminal_controller, SmartTerminal *st, TrainController *train_controller);

