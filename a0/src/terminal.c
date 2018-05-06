#include <bwio.h>
#include <io.h>
#include <terminal.h>

// Util functions to be refactored later
int strncmp(const char *s, const char *t, int n) {
    while(n--) {
        if (*s++ != *t++) {
            return *(unsigned char*)(s - 1) - *(unsigned char*)(t - 1); 
        } 
    }
    return 0;
}

void tc_init(TerminalController *controller, SmartTerminal *st) {
    controller->st = st;
}

int tc_process_terminal_input(TerminalController *controller) {
    return st_process_terminal_input(controller->st, controller);
}

int tc_process_time(TerminalController *controller, Clock *clock) {
    if (cl_time_changed(clock)) {
        tc_update_time(controller, cl_get_time_ms(clock));
    }
    return 0;
}


void tc_render_static(TerminalController *controller) {
    BufferedChannel *channel =  &(controller->st->channel);

    st_clear_screen(controller->st);

    // Clock
    st_move_cursor_top_left(controller->st);
    printf(channel, "Clock: ");

    // Command status
    st_move_cursor(controller->st, STATUS_ROW, START_COL);
    printf(channel, "Status: ");

    // Command history
    st_move_cursor(controller->st, HISTORY_ROW, START_COL);
    printf(channel, "History: ");

    // Command prompt
    st_move_cursor(controller->st, PROMPT_ROW, START_COL);
    printf(channel, "Prompt: ");

    // Move to prompt
    st_move_cursor(controller->st, PROMPT_ROW, DYNAMIC_COL);
    return;
}

int tc_update_command(TerminalController *controller, char *command) {
    st_save_cursor(controller->st);

    // Update command status
    st_move_cursor(controller->st, STATUS_ROW, DYNAMIC_COL);
    st_clear_line_from_cursor(controller->st);

    if (strncmp(command, "q", 1) == 0) {
        return 1;
    } else if (strncmp(command, "tr", 2) == 0) {
        printf(&(controller->st->channel), "Got tr");
    } else if (strncmp(command, "rv", 2) == 0) {
        printf(&(controller->st->channel), "Got rv");
    } else if (strncmp(command, "sw", 2) == 0) {
        printf(&(controller->st->channel), "Got sw");
    } else {
        printf(&(controller->st->channel), "Got unkown command");
    }

    // Update command history
    st_move_cursor(controller->st, HISTORY_ROW, DYNAMIC_COL);
    st_clear_line_from_cursor(controller->st);
    putstr(&(controller->st->channel), command);

    st_restore_cursor(controller->st);
    return 0;
}

int tc_update_time(TerminalController *controller, long time_ms) {
    st_save_cursor(controller->st);
    st_move_cursor(controller->st, 0, 10);

    int time = time_ms;
    int ms = time % 1000;
    time /= 1000;
    int s = time % 60;
    time /= 60;
    int m = time % 60;
    int h = time / 60;
    printf(&(controller->st->channel), "%d:%d:%d:%d\r\n", h, m, s, ms);

    st_restore_cursor(controller->st);

    return 0;
}

int tc_tr(int train_number, int train_speed) {
    return 0;
}

int tc_rv(int train_number) {
    return 0;
}

int tc_sw(int switch_number, int switch_direction) {
    return 0;
}

/*
 * Smart Terminal
 */

void st_init(SmartTerminal *st, int id) {
    bc_init(&(st->channel), id);
    st->size = 0;
}

void st_poll(SmartTerminal *st) {
    // Write from write buffer to URT
    if (!rb_is_empty(&(st->channel.writeBuffer))) {
        put(&(st->channel));
    }

    // Read from URT to read buffer
    get(&(st->channel));
}

int st_process_terminal_input(SmartTerminal *st, TerminalController *controller) {
    BufferedChannel *channel =  &(st->channel);
    if (!rb_is_empty(&(channel->readBuffer))) {
        char ch = getc(channel);

        // Echo visible input from terminal
        if (ch >= CHAR_VISIBLE_START && ch <= CHAR_VISIBLE_END) {
            putc(channel, ch);

            // Add to command buffer
            // TODO(sdsuo): Check for overflow here
            st->commandBuffer[st->size] = ch;
            st->size ++;
        } else if (ch == CHAR_ENTER) {
            st->commandBuffer[st->size] = '\0';
            st_move_cursor(controller->st, 32, 10);
            st_clear_line_from_cursor(st);

            int exit = tc_update_command(controller, st->commandBuffer);

            // NOTE(sdsuo): Wipe out the command buffer
            st->size = 0;
            
            return exit;
        } else if (ch == CHAR_BACKSPACE) {
            st_clear_line_from_cursor(st);
            if (st->size > 0) {
                putc(channel, ch);
                st->size --; 
            }
        } else {
            // TODO(sdsuo): Handle other cases (e.g. backspace)
        }
    } 
    return 0;
}

void st_save_cursor(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B""7");
}

void st_restore_cursor(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B""8");
}

void st_move_cursor_top_left(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[H");
}

void st_move_cursor(SmartTerminal *st, int vertical, int horizontal) {
    printf(&(st->channel), "\x1B[%d;%dH", vertical, horizontal);
}

void st_clear_line_from_cursor(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[K");
}

void st_clear_line(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[2K");
}

void st_clear_screen(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[2J");
}


