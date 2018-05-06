#include <bwio.h>
#include <io.h>
#include <train.h>
#include <util.h>

/*
 * Set communication protocol:
 * Baud rate = 2400
 * Start bits (if requested by computer) = 1
 * Stop bits = 2
 * Parity = None
 * Word size = 8 bits
 * FIFO = OFF
 */
void tr_init_protocol(BufferedChannel *channel) {
    setspeed(channel, 2400);
    set2stopbits(channel);
    setnoparity(channel);
    set8wordsize(channel);
    setfifo(channel, 0);
}

void tr_update_command(BufferedChannel *channel, char *command) {
    if (strncmp(command, "tr", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        int train_speed = parse_int_arg(command, 1);
        tr_tr(channel, train_number, train_speed);
    } else if (strncmp(command, "rv", 2) == 0) {
        int train_number = parse_int_arg(command, 0);
        tr_rv(channel, train_number);
    } else if (strncmp(command, "sw", 2) == 0) {
    } else if (strncmp(command, "go", 2) == 0) {
        tr_go(channel);
    } else if (strncmp(command, "stop", 4) == 0) {
        tr_stop(channel);
    }
}

void tr_go(BufferedChannel *channel) {
    putc(channel, TRAIN_GO);
}

void tr_stop(BufferedChannel *channel) {
    putc(channel, TRAIN_STOP);
}

int tr_tr(BufferedChannel *channel, int train_number, int train_speed) {
    putc(channel, (char) train_speed);
    putc(channel, (char) train_number);
    return 0;
}

int tr_rv(BufferedChannel *channel, int train_number) {
    putc(channel, (char) TRAIN_SPEED_REVERSE);
    putc(channel, (char) train_number);
    return 0;
}

int tr_sw(BufferedChannel *channel, int switch_number, int switch_direction) {
    return 0;
}
