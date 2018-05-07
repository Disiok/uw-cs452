#include <bwio.h>
#include <io.h>
#include <train.h>
#include <util.h>

void tr_init(TrainController *controller, Clock *clock) {
    bc_init(&(controller->channel), COM1);
    rb_init(&(controller->rvBuffer));
    rb_init(&(controller->swBuffer));
    controller->clock = clock;

    tr_init_protocol(controller);
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

    setspeed(channel, 2400);
    set2stopbits(channel);
    setnoparity(channel);
    set8wordsize(channel);
    setfifo(channel, 0);
}

void tr_poll(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);
    RingBuffer *swBuffer = &(controller->swBuffer);
    RingBuffer *rvBuffer = &(controller->rvBuffer);
    
    if (cl_time_changed(controller->clock)) {
        int time_ms = cl_get_time_ms(controller->clock);            

        // Check switch buffer
        int t_0 = rb_peak(swBuffer, 0);
        int t_1 = rb_peak(swBuffer, 1);
        int t_2 = rb_peak(swBuffer, 2);
        int t_3 = rb_peak(swBuffer, 3);
        
        int scheduled_time =  t_0 + t_1 << 8 + t_2 << 16 + t_3 << 24;

        if (scheduled_time < time_ms) {
            rb_shrink(swBuffer);
            rb_shrink(swBuffer);
            rb_shrink(swBuffer);
            rb_shrink(swBuffer);
            char command = rb_shrink(swBuffer);
        
            putc(channel, command);
        }

        // Check reverse buffer
        t_0 = rb_peak(rvBuffer, 0);
        t_1 = rb_peak(rvBuffer, 1);
        t_2 = rb_peak(rvBuffer, 2);
        t_3 = rb_peak(rvBuffer, 3);
        
        scheduled_time =  t_0 + t_1 << 8 + t_2 << 16 + t_3 << 24;

        if (scheduled_time < time_ms) {
            rb_shrink(rvBuffer);
            rb_shrink(rvBuffer);
            rb_shrink(rvBuffer);
            rb_shrink(rvBuffer);
            char command = rb_shrink(rvBuffer);
        
            putc(channel, command);
        }
    }
}

void tr_update_command(TrainController *controller, char *command) {
    if (strncmp(command, "tr", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        int train_speed = parse_int_arg(command, 1);
        tr_tr(controller, train_number, train_speed);
    } else if (strncmp(command, "rv", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        tr_rv(controller, train_number);
    } else if (strncmp(command, "sw", 2) == 0) {
        int switch_number = parse_int_arg(command, 0);
        char switch_direction = parse_char_arg(command, 1);
        tr_sw(controller, switch_number, switch_direction);
    } else if (strncmp(command, "go", 2) == 0) {
        tr_go(controller);
    } else if (strncmp(command, "stop", 4) == 0) {
        tr_stop(controller);
    }
}

void tr_go(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);
    putc(channel, TRAIN_GO);
}

void tr_stop(TrainController *controller) {
    BufferedChannel *channel = &(controller->channel);
    putc(channel, TRAIN_STOP);
}

int tr_tr(TrainController *controller, int train_number, int train_speed) {
    BufferedChannel *channel = &(controller->channel);
    putc(channel, (char) train_speed);
    putc(channel, (char) train_number);
    return 0;
}

int tr_rv(TrainController *controller, int train_number) {
    BufferedChannel *channel = &(controller->channel);

    putc(channel, (char) 0);
    putc(channel, (char) train_number);

    tr_schedule_delayed_rv(controller, train_number);
    return 0;
}

int tr_sw(TrainController *controller, int switch_number, char switch_direction) {
    BufferedChannel *channel = &(controller->channel);

    if (switch_direction == 's') {
        putc(channel, TRAIN_SWITCH_STRAIGHT); 
        putc(channel, (char) switch_number);
    } else if (switch_direction == 'c') {
        putc(channel, TRAIN_SWITCH_CURVE); 
        putc(channel, (char) switch_number);
    }

    tr_schedule_delayed_sw(controller);
    return 0;
}

void tr_put_scheduled_time(RingBuffer *buffer, long time_ms) {
    rb_grow(buffer, (char) time_ms & 0xFF);
    rb_grow(buffer, (char) (time_ms>>8)& 0xFF);
    rb_grow(buffer, (char) (time_ms>>16)& 0xFF);
    rb_grow(buffer, (char) (time_ms>>24)& 0xFF);

    return;
}

void tr_schedule_delayed_sw(TrainController *controller) {
    int time_ms = cl_get_time_ms(controller->clock) + TRAIN_SW_DELAY;

    // Turn switch solenoid off
    tr_put_scheduled_time(&(controller->swBuffer), time_ms);
    rb_grow(&(controller->swBuffer), TRAIN_SWITCH_OFF);
    return;
}

void tr_schedule_delayed_rv(TrainController *controller, int train_number) {
    int time_ms = cl_get_time_ms(controller->clock) + TRAIN_RV_DELAY;
    // Reverse
    tr_put_scheduled_time(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) TRAIN_SPEED_REVERSE);
    tr_put_scheduled_time(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) train_number);

    // Speed up
    tr_put_scheduled_time(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) 10);
    tr_put_scheduled_time(&(controller->rvBuffer), time_ms);
    rb_grow(&(controller->rvBuffer), (char) train_number);
}

