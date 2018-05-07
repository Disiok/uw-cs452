
#pragma once


#include <time.h>
#include <ds.h>
#include <io.h>

#define TRAIN_GO 0x60
#define TRAIN_STOP 0x61
#define TRAIN_SPEED_MAX 14
#define TRAIN_SPEED_MIN 0
#define TRAIN_SPEED_REVERSE 15

#define TRAIN_SWITCH_OFF 0x20
#define TRAIN_SWITCH_STRAIGHT 0x21
#define TRAIN_SWITCH_CURVE 0x22

#define TRAIN_SW_DELAY 150
#define TRAIN_RV_DELAY 3000


/**
 * Train Controller: controller for train set
 *
 * This contains logic for:
 * 1. Issuing commands
 * 2. Scheduling delayed commands
 */
typedef struct {
    BufferedChannel channel;
    RingBuffer swBuffer;
    RingBuffer rvBuffer;
    Clock *clock;
} TrainController;

void tr_init(TrainController *controller, Clock *clock);
void tr_init_protocol(TrainController *controller);

void tr_poll(TrainController *controller);

void tr_update_command(TrainController *controller, char *command);

// Train set functions
void tr_go(TrainController *controller);
void tr_stop(TrainController *controller);
int tr_tr(TrainController *controller, int tcain_number, int tcain_speed);
int tr_rv(TrainController *controller, int tcain_number);
int tr_sw(TrainController *controller, int switch_number, char switch_direction);

// Delayed train set functions
void tr_schedule_delayed_sw(TrainController *controller);
void tr_schedule_delayed_rv(TrainController *controller, int train_number);
