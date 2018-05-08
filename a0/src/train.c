#include <bwio.h>
#include <io.h>
#include <train.h>
#include <util.h>

void tr_init(TrainController *controller, Clock *clock, Track *track) {
    controller->clock = clock;
    bc_init(&(controller->channel), COM1);
    rb_init(&(controller->rvBuffer));
    rb_init(&(controller->swBuffer));
    rb_init(&(controller->sensorBuffer));
    controller->track = track;

    // Only display 20 most recent
    rb_set_max_size(&(controller->sensorBuffer), TRAIN_SENSOR_DISPLAY_MAX);

    // Enable sensor polling by default
    controller->sensorFlag = 1;

    // Set up communication protocol
    tr_init_protocol(controller);

    // Initialize train speed
    tr_init_train_speed(controller);

    // Initialize switch positions
    tr_switch_all(controller, track, 'S');

    if (controller->sensorFlag) {
        tr_request_sensors(controller, 5);
    }
}

/*
 * Set communication protocol:
 * Baud rate = 2400
 * Start bits (if requested by computer) = 1
 * Stop bits = 2
 * Parity = None
 * Word size = 8 bits
 * FIFO = OFF
 */
void tr_init_protocol(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);

    setspeed(channel, TRAIN_COM_SPEED);
    set2stopbits(channel);
    setnoparity(channel);
    set8wordsize(channel);
    setfifo(channel, 0);
}

void tr_init_train_speed(TrainController *controller) {
    int i;
    for (i = TRAIN_NUMBER_MIN; i < TRAIN_NUMBER_MAX; i++) {
        controller->trainSpeed[i] = TRAIN_SPEED_MIN;    
    }

    return;
}

void tr_poll(TrainController *controller, SmartTerminal *st) {
    BufferedChannel *channel = &(controller->channel);
    bc_poll(channel);

    if (cl_time_changed(controller->clock)) {
        int time_ms = cl_get_time_ms(controller->clock);            

        // Check switch buffer
        RingBuffer *swBuffer = &(controller->swBuffer);
        if (!rb_is_empty(swBuffer)) {
            int sw_scheduled_time = rb_peak_int(swBuffer);

            if (sw_scheduled_time < time_ms) {
                st_update_status(st, "Sending delayed switch");
                rb_shrink_int(swBuffer);
                char command = rb_shrink(swBuffer);
                putc(channel, command);
            }
        }

        // Check reverse buffer
        RingBuffer *rvBuffer = &(controller->rvBuffer);
        if (!rb_is_empty(rvBuffer)) {
            int rv_scheduled_time = rb_peak_int(rvBuffer);
            // bwprintf(COM2, "Waiting time: %d", rv_scheduled_time - time_ms);

            if (rv_scheduled_time < time_ms) {
                st_update_status(st, "Sending delayed reverse");
                rb_shrink_int(rvBuffer);
                char command = rb_shrink(rvBuffer);
                // bwprintf(COM2, "[Char is %d]", command);
                putc(channel, command);
            }
        }

        // Update sensor display
        if (!rb_is_empty(&(channel->readBuffer))) {
            int i;
            int j;
            int k;
            for (i = 0; i < TRAIN_SENSOR_MAX; i ++) {
                // Every sensor group
                for (j = 0; j < 2; j ++) {
                    // Every byte
                    char sensor_byte = rb_shrink(&(channel->readBuffer));
                    for (k = 0; k < 8; k ++) { 
                        // Every bit
                        char sensor_bit = ((sensor_byte >> k) & 1);
                        if (sensor_bit) {
                            // From 16 -> 9 -> 8 -> 1
                            // char sensor_id = (i + 1) * 16 - (j + 1) * 8 + k + 1;

                            // From 1 - > 8 -> 9 -> 16
                            char sensor_id = i * 16 + (j + 1) * 8 - k - 1;
                            rb_grow(&(controller->sensorBuffer), sensor_id);
                        }
                    }
                }

            }

            st_update_sensors(st, &(controller->sensorBuffer), controller->sensorFlag);      
        } else {
            // poll sensors
            if (controller->sensorFlag) {
                tr_request_sensors(controller, TRAIN_SENSOR_MAX);
            }
        }
    }
}

void tr_update_command(TrainController *controller, char *command, SmartTerminal *st) {
    if (strncmp(command, "tr", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        int train_speed = parse_int_arg(command, 1);
        tr_set_speed(controller, train_number, train_speed);
    } else if (strncmp(command, "rv", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        tr_reverse(controller, train_number);
    } else if (strncmp(command, "swa", 3) == 0) {
        char switch_direction = parse_char_arg(command, 0);
        int failed = tr_switch_all(controller, controller->track, switch_direction);

        if (!failed) {
            st_update_switch_all(st, switch_direction, controller->track);
        }
    } else if (strncmp(command, "sw", 2) == 0) {
        int switch_number = parse_int_arg(command, 0);
        char switch_direction = parse_char_arg(command, 1);
        int failed = tr_switch(controller, switch_number, switch_direction, 1);

        if (!failed) {
            st_update_switch(st, switch_number, switch_direction, controller->track);
        }
    } else if (strncmp(command, "go", 2) == 0) {
        tr_go(controller);
    } else if (strncmp(command, "stop", 4) == 0) {
        tr_stop(controller);
    } else if (strncmp(command, "ss", 2) == 0) {
        tr_toggle_sensor(controller); 
    }
}

int tr_toggle_sensor(TrainController *controller) {
   controller->sensorFlag = 1 - controller->sensorFlag; 
   return 0;
}

int tr_go(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);
    putc(channel, TRAIN_GO);

    return 0;
}

int tr_stop(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);
    putc(channel, TRAIN_STOP);

    return 0;
}

int tr_set_speed(TrainController *controller, int train_number, int train_speed) {
    if (train_speed > TRAIN_SPEED_REVERSE || train_speed < TRAIN_SPEED_MIN) {
        return 1; 
    }

    if (train_number > TRAIN_NUMBER_MAX || train_number < TRAIN_NUMBER_MIN) {
        return 1; 
    }

    controller->trainSpeed[train_number] = (char) train_speed;

    BufferedChannel *channel = &(controller->channel);
    putc(channel, (char) train_speed);
    putc(channel, (char) train_number);
    return 0;
}

int tr_reverse(TrainController *controller, int train_number) {
    if (train_number > TRAIN_NUMBER_MAX || train_number < TRAIN_NUMBER_MIN) {
        return 1; 
    }

    BufferedChannel *channel = &(controller->channel);
    putc(channel, (char) 0);
    putc(channel, (char) train_number);

    return tr_schedule_delayed_reverse(controller, train_number);
}

int tr_request_sensors(TrainController *controller, int max) {
    BufferedChannel *channel = &(controller->channel);

    putc(channel, TRAIN_SENSOR_BASE + max);

    return 0;
}

int tr_switch(TrainController *controller, int switch_number, char switch_direction, int delayed) {
    if (switch_number > TRAIN_SWITCH_MAX || switch_number < TRAIN_SWITCH_MIN) {
        return 1; 
    }

    BufferedChannel *channel = &(controller->channel);
    if (switch_direction == 'S') {
        putc(channel, TRAIN_SWITCH_STRAIGHT); 
        putc(channel, (char) switch_number);
    } else if (switch_direction == 'C') {
        putc(channel, TRAIN_SWITCH_CURVE); 
        putc(channel, (char) switch_number);
    } else {
        return 1;
    }

    if (delayed) {
        return tr_schedule_delayed_switch(controller);
    } else {
        return 0; 
    }
}

int tr_switch_all(TrainController *controller, Track *track, char state) {
    int i;
    for (i = 0; i < TRAIN_SWITCH_MAX ; i ++) {
        if (track->switch_exist[i] != -1) {
            tr_switch(controller, i, state, 0);
        }
    }
    return tr_schedule_delayed_switch(controller);
}


int tr_schedule_delayed_switch(TrainController *controller) {
    int time_ms = cl_get_time_ms(controller->clock) + TRAIN_SW_DELAY;

    // Turn switch solenoid off
    rb_grow_int(&(controller->swBuffer), time_ms);
    rb_grow(&(controller->swBuffer), TRAIN_SWITCH_OFF);

    return 0;
}

int tr_schedule_delayed_reverse(TrainController *controller, int train_number) {
    int time_ms = cl_get_time_ms(controller->clock) + TRAIN_RV_DELAY;

    // Reverse
    rb_grow_int(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) TRAIN_SPEED_REVERSE);
    rb_grow_int(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) train_number);

    // Speed up
    rb_grow_int(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) controller->trainSpeed[train_number]);
    rb_grow_int(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) train_number);

    return 0;
}

