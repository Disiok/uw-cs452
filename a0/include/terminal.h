
#pragma once 

typedef struct SmartTerminal SmartTerminal;
typedef struct TerminalController TerminalController;

#include <io.h>
#include <train.h>
#include <time.h>
#include <track_node.h>

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
void st_render_static(SmartTerminal *st, track_node *track);
int st_update_command(SmartTerminal *st, char *command);
int st_update_time(SmartTerminal *st, long time_ms);
int st_update_sensors(SmartTerminal *st, char *sensors);

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

