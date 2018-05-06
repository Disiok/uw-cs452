
#pragma once

#include <io.h>

#define TRAIN_GO 0x60
#define TRAIN_STOP 0x61
#define TRAIN_SPEED_MAX 14
#define TRAIN_SPEED_MIN 0
#define TRAIN_SPEED_REVERSE 15


void tr_init_protocol(BufferedChannel *channel);
void tr_update_command(BufferedChannel *channel, char *command);

void tr_go(BufferedChannel *channel);

void tr_stop(BufferedChannel *channel);

int tr_tr(BufferedChannel *channel, int train_number, int train_speed);

int tr_rv(BufferedChannel *channel, int train_number);

int tr_sw(BufferedChannel *channel, int switch_number, int switch_direction);