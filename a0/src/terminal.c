#include <bwio.h>
#include <io.h>
#include <util.h>
#include <train.h>
#include <track.h>
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
            st_move_cursor(st, PROMPT_ROW, 10);
            st_clear_line_from_cursor(st);

            int exit = st_update_command(st, terminal_controller->commandBuffer);
            tr_update_command(train_controller, terminal_controller->commandBuffer, st);

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

void st_render_static(SmartTerminal *st, Track *track) {
    BufferedChannel *channel =  &(st->channel);

    st_clear_screen(st);

    // Clock
    st_move_cursor_top_left(st);
    printf(channel, "Clock: ");

    int i;
    // Switches
    st_move_cursor(st, SWITCHES_ROW_START - 1, SWITCHES_COL_START);
    putstr(channel, "Switches: ");

    for (i = 0; i < TRAIN_SWITCH_MAX; i ++) {
        if (track->switch_exist[i] != -1) {
            int switch_position = track->switch_exist[i];
            st_move_cursor(st, SWITCHES_ROW_START + switch_position % 15, SWITCHES_COL_START + switch_position / 15 * 20);
            printf(channel, "[S] %d: %s %s ", i, track->merge_names[i], track->branch_names[i]); 
        }
    }
    
    // Sensors
    /*
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
    */

    st_move_cursor(st, SENSORS_ROW_START - 1, SENSORS_COL_START);
    putstr(channel, "Sensor Updates: ");

    // Command status
    st_move_cursor(st, STATUS_ROW, START_COL);
    putstr(channel, "Status: ");

    // Command history
    st_move_cursor(st, HISTORY_ROW, START_COL);
    putstr(channel, "History: ");

    // Command prompt
    st_move_cursor(st, PROMPT_ROW, START_COL);
    putstr(channel, ">");

    // Move to prompt
    st_move_cursor(st, PROMPT_ROW, DYNAMIC_COL);
    return;
}

int st_update_status(SmartTerminal *st, char *status) {
    st_save_cursor(st);
    st_move_cursor(st, STATUS_ROW, DYNAMIC_COL);
    st_clear_line_from_cursor(st);
    putstr(&(st->channel), status);
    st_restore_cursor(st);

    return 0;
}

int st_update_history(SmartTerminal *st, char *history) {
    st_save_cursor(st);
    st_move_cursor(st, HISTORY_ROW, DYNAMIC_COL);
    st_clear_line_from_cursor(st);
    putstr(&(st->channel), history);
    st_restore_cursor(st);

    return 0;
}


int st_update_command(SmartTerminal *st, char *command) {
    if (strncmp(command, "q", 1) == 0) {
        // Exit
        return 1;
    } else {
        // Update command history
        st_update_history(st, command);

        // Update status
        if (strncmp(command, "tr", 2) == 0) {
            st_update_status(st, "Got tr command");
        } else if (strncmp(command, "rv", 2) == 0) {
            st_update_status(st, "Got rv command");
        } else if (strncmp(command, "sw", 2) == 0) {
            st_update_status(st, "Got sw command");
        } else if (strncmp(command, "go", 2) == 0) {
            st_update_status(st, "Got go command");
        } else if (strncmp(command, "stop", 4) == 0) {
            st_update_status(st, "Got stop command");
        } else {
            st_update_status(st, "Got unknown command");
        }

        return 0;
    }
}

int st_update_time(SmartTerminal *st, int time_ms) {
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

int st_update_switch(SmartTerminal *st, int switch_number, char state, Track *track) {
    st_save_cursor(st);
    if (track->switch_exist[switch_number] != -1) {
        int switch_position = track->switch_exist[switch_number];
        st_move_cursor(st, SWITCHES_ROW_START + switch_position % 15, SWITCHES_COL_START + switch_position / 15 * 20 + 1);
        putc(&(st->channel), state); 
    }
    st_restore_cursor(st);

    return 0;
}

int st_update_switch_all(SmartTerminal *st, char state, Track *track) {
    int i;
    for (i = 0; i < TRAIN_SWITCH_MAX; i ++) {
        if (track->switch_exist[i] != -1) {
            st_update_switch(st, i, state, track);
        }
    }
    return 0;
}

int st_update_sensors(SmartTerminal *st, RingBuffer *sensorBuffer, int sensorFlag) {
    st_save_cursor(st);
    st_move_cursor(st, SENSORS_ROW_START - 1, SENSORS_COL_START + 17);
    if (sensorFlag) {
        putstr(&(st->channel), " On");
    } else {
        putstr(&(st->channel), "Off");
    }
    st_move_cursor(st, SENSORS_ROW_START, SENSORS_COL_START);

    int i;
    char activated;
    for (i = sensorBuffer->size - 1; i >= 0; i --) {
        activated = rb_peak(sensorBuffer, i);
        
        int display_i = sensorBuffer->size - 1 - i;
        st_move_cursor(st, SENSORS_ROW_START + display_i % 10, 50 + display_i / 10 * 5);
        putstr(&(st->channel), "     ");
        st_move_cursor(st, SENSORS_ROW_START + display_i % 10, 50 + display_i / 10 * 5);
        putc(&(st->channel), ('A' + activated / 16)); 
        printf(&(st->channel), "%d", activated % 16 + 1); 
    }

    st_restore_cursor(st);

    return 0;
}
