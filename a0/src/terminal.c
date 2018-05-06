#include <bwio.h>
#include <io.h>
#include <util.h>
#include <train.h>
#include <track_data.h>
#include <terminal.h>

void tc_init(TerminalController *controller, SmartTerminal *st) {
    controller->st = st;
}

int tc_process_time(TerminalController *controller, Clock *clock) {
    if (cl_time_changed(clock)) {
        tc_update_time(controller, cl_get_time_ms(clock));
    }
    return 0;
}


void tc_render_static(TerminalController *controller, track_node *track) {
    BufferedChannel *channel =  &(controller->st->channel);

    st_clear_screen(controller->st);

    // Clock
    st_move_cursor_top_left(controller->st);
    printf(channel, "Clock: ");

    int i;
    // Switches
    st_move_cursor(controller->st, SWITCHES_ROW_START - 1, SWITCHES_COL_START);
    putstr(channel, "Switches");
    int switches = 0;
    for (i = 0; i < TRACK_MAX; i++) {
        if (track[i].type == NODE_BRANCH) {
            st_move_cursor(controller->st, SWITCHES_ROW_START + switches % 10, SWITCHES_COL_START + switches / 10 * 5);
            putstr(channel, track[i].name);
            switches ++;
        }
    }
    
    // Sensors
    st_move_cursor(controller->st, SENSORS_ROW_START - 1, SENSORS_COL_START);
    putstr(channel, "Sensors");
    int sensors = 0;
    for (i = 0; i < TRACK_MAX; i++) {
        if (track[i].type == NODE_SENSOR) {
            st_move_cursor(controller->st, SENSORS_ROW_START + sensors % 10, SENSORS_COL_START + sensors / 10 * 5);
            putstr(channel, track[i].name);
            sensors ++;
        }
    }

    // Command status
    st_move_cursor(controller->st, STATUS_ROW, START_COL);
    printf(channel, "Status: ");

    // Command history
    st_move_cursor(controller->st, HISTORY_ROW, START_COL);
    printf(channel, "History: ");

    // Command prompt
    st_move_cursor(controller->st, PROMPT_ROW, START_COL);
    printf(channel, ">");


    // Move to prompt
    st_move_cursor(controller->st, PROMPT_ROW, DYNAMIC_COL);
    return;
}

int tc_update_command(TerminalController *controller, char *command) {
    if (strncmp(command, "q", 1) == 0) {
        // Exit
        return 1;
    } else {
        st_save_cursor(controller->st);

        // Update command status
        st_move_cursor(controller->st, STATUS_ROW, DYNAMIC_COL);
        st_clear_line_from_cursor(controller->st);

        if (strncmp(command, "tr", 2) == 0) {
            printf(&(controller->st->channel), "Got tr");
        } else if (strncmp(command, "rv", 2) == 0) {
            printf(&(controller->st->channel), "Got rv");
        } else if (strncmp(command, "sw", 2) == 0) {
            printf(&(controller->st->channel), "Got sw");
        } else if (strncmp(command, "go", 2) == 0) {
            printf(&(controller->st->channel), "Got go");
        } else if (strncmp(command, "stop", 4) == 0) {
            printf(&(controller->st->channel), "Got stop");
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

/*
 * Smart Terminal
 */

void st_init(SmartTerminal *st, int id) {
    bc_init(&(st->channel), id);
    st->size = 0;
}

void st_poll(SmartTerminal *st) {
    bc_poll(&(st->channel));
}

int st_process_terminal_input(SmartTerminal *st, TerminalController *controller, BufferedChannel *train_channel) {
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

            tr_update_command(train_channel, st->commandBuffer);
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


