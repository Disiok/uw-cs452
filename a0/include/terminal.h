
#pragma once 

#include <io.h>
#include <time.h>

/*
 * Smart Terminal: wrapper on gtk for moving cursors around
 *
 * This contains logic for:
 * 1. Basic functions dealing with cursors
 */

#define CHAR_ENTER 0x0D
#define CHAR_BACKSPACE 0x08
#define CHAR_VISIBLE_START 0x20
#define CHAR_VISIBLE_END 0x7E
#define CHAR_QUIT 'q'

#define STATUS_ROW 30
#define HISTORY_ROW 31
#define PROMPT_ROW 32

#define START_COL 0
#define DYNAMIC_COL 10

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
typedef struct _TerminalController {
    SmartTerminal *st;
} TerminalController;

void tc_init(TerminalController *controller, SmartTerminal *st);
int tc_process_time(TerminalController *controller, Clock *clock);
// Following functions for formatting and positioning
void tc_render_static(TerminalController *controller);
int tc_update_time(TerminalController *controller, long time_ms);
int tc_update_sensor(TerminalController *controller);

void st_init(SmartTerminal *st, int id);
void st_poll(SmartTerminal *st);
int st_process_terminal_input(SmartTerminal *st, TerminalController *controller, BufferedChannel *train_channel);
void st_save_cursor(SmartTerminal *st);
void st_restore_cursor(SmartTerminal *st);
void st_move_cursor(SmartTerminal *st, int vertical, int horizontal);
void st_move_cursor_top_left(SmartTerminal *st);
void st_clear_line_from_cursor(SmartTerminal *st);
void st_clear_line(SmartTerminal *st);
void st_clear_screen(SmartTerminal *st);

