#include <bwio.h>
#include <io.h>
#include <util.h>
#include <train.h>
#include <track_data.h>
#include <terminal.h>

void tc_init(TerminalController *controller) {
    controller->size = 0;
}

int tc_poll(TerminalController *terminal_controller, SmartTerminal *st, TrainController *train_controller) {
    BufferedChannel *channel =  &(st->channel);
    if (!rb_is_empty(&(channel->readBuffer))) {
        char ch = getc(channel);

        // Echo visible input from terminal
        if (ch >= CHAR_VISIBLE_START && ch <= CHAR_VISIBLE_END) {
            putc(channel, ch);

            // Add to command buffer
            // TODO(sdsuo): Check for overflow here
            terminal_controller->commandBuffer[terminal_controller->size] = ch;
            terminal_controller->size ++;
        } else if (ch == CHAR_ENTER) {
            terminal_controller->commandBuffer[terminal_controller->size] = '\0';
            st_move_cursor(st, 32, 10);
            st_clear_line_from_cursor(st);

            tr_update_command(train_controller, terminal_controller->commandBuffer);
            int exit = st_update_command(st, terminal_controller->commandBuffer);

            // NOTE(sdsuo): Wipe out the command buffer
            terminal_controller->size = 0;
            
            return exit;
        } else if (ch == CHAR_BACKSPACE) {
            st_clear_line_from_cursor(st);
            if (terminal_controller->size > 0) {
                putc(channel, ch);
                terminal_controller->size --; 
            }
        } else {
            // TODO(sdsuo): Handle other cases (e.g. backspace)
        }
    } 
    return 0;
}


/*
 * Smart Terminal
 */

void st_init(SmartTerminal *st) {
    bc_init(&(st->channel), COM2);
}

void st_poll(SmartTerminal *st) {
    bc_poll(&(st->channel));
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

void st_render_static(SmartTerminal *st, track_node *track) {
    BufferedChannel *channel =  &(st->channel);

    st_clear_screen(st);

    // Clock
    st_move_cursor_top_left(st);
    printf(channel, "Clock: ");

    int i;
    // Switches
    st_move_cursor(st, SWITCHES_ROW_START - 1, SWITCHES_COL_START);
    putstr(channel, "Switches");
    int switches = 0;
    for (i = 0; i < TRACK_MAX; i++) {
        if (track[i].type == NODE_BRANCH || track[i].type == NODE_MERGE) {
            st_move_cursor(st, SWITCHES_ROW_START + switches % 20, SWITCHES_COL_START + switches / 20 * 10);
            printf(channel, "%s: %d", track[i].name, track[i].num);
            switches ++;
        }
    }
    
    // Sensors
    st_move_cursor(st, SENSORS_ROW_START - 1, SENSORS_COL_START);
    putstr(channel, "Sensors");
    int sensors = 0;
    for (i = 0; i < TRACK_MAX; i++) {
        if (track[i].type == NODE_SENSOR) {
            st_move_cursor(st, SENSORS_ROW_START + sensors % 20, SENSORS_COL_START + sensors / 20 * 10);
            printf(channel, "%s: %d", track[i].name, track[i].num);
            sensors ++;
        }
    }

    // Command status
    st_move_cursor(st, STATUS_ROW, START_COL);
    printf(channel, "Status: ");

    // Command history
    st_move_cursor(st, HISTORY_ROW, START_COL);
    printf(channel, "History: ");

    // Command prompt
    st_move_cursor(st, PROMPT_ROW, START_COL);
    printf(channel, ">");


    // Move to prompt
    st_move_cursor(st, PROMPT_ROW, DYNAMIC_COL);
    return;
}


int st_update_command(SmartTerminal *st, char *command) {
    if (strncmp(command, "q", 1) == 0) {
        // Exit
        return 1;
    } else {
        st_save_cursor(st);

        // Update command status
        st_move_cursor(st, STATUS_ROW, DYNAMIC_COL);
        st_clear_line_from_cursor(st);

        if (strncmp(command, "tr", 2) == 0) {
            printf(&(st->channel), "Got tr");
        } else if (strncmp(command, "rv", 2) == 0) {
            printf(&(st->channel), "Got rv");
        } else if (strncmp(command, "sw", 2) == 0) {
            printf(&(st->channel), "Got sw");
        } else if (strncmp(command, "go", 2) == 0) {
            printf(&(st->channel), "Got go");
        } else if (strncmp(command, "stop", 4) == 0) {
            printf(&(st->channel), "Got stop");
        } else {
            printf(&(st->channel), "Got unkown command");
        }

        // Update command history
        st_move_cursor(st, HISTORY_ROW, DYNAMIC_COL);
        st_clear_line_from_cursor(st);
        putstr(&(st->channel), command);

        st_restore_cursor(st);
        return 0;
    }
}

int st_update_time(SmartTerminal *st, long time_ms) {
    st_save_cursor(st);
    st_move_cursor(st, 0, 10);

    int time = time_ms;
    int ms = time % 1000;
    time /= 1000;
    int s = time % 60;
    time /= 60;
    int m = time % 60;
    int h = time / 60;
    printf(&(st->channel), "%d:%d:%d:%d\r\n", h, m, s, ms);

    st_restore_cursor(st);

    return 0;
}

int st_update_sensors(SmartTerminal *st, char *sensors) {
    return 0;
}
