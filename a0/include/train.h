
#pragma once


#include <time.h>
#include <ds.h>
#include <io.h>

// Train set
#define TRAIN_GO 0x60
#define TRAIN_STOP 0x61

// Trains
#define TRAIN_SPEED_MAX 14
#define TRAIN_SPEED_MIN 0
#define TRAIN_SPEED_REVERSE 15

#define TRAIN_NUMBER_MAX 80
#define TRAIN_NUMBER_MIN 1

// Switchs
#define TRAIN_SWITCH_MAX 255
#define TRAIN_SWITCH_MIN 0

#define TRAIN_SWITCH_OFF 0x20
#define TRAIN_SWITCH_STRAIGHT 0x21
#define TRAIN_SWITCH_CURVE 0x22

// Sensors
#define TRAIN_SENSOR_BASE 0x80

// Delays
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
    char trainSpeed[TRAIN_NUMBER_MAX + 1];
} TrainController;

#include <terminal.h>

void tr_init(TrainController *controller, Clock *clock);
void tr_init_protocol(TrainController *controller);
void tr_init_train_speed(TrainController *controller);
void tr_poll(TrainController *controller, TerminalController *terminal);
void tr_update_command(TrainController *controller, char *command);

// Train set functions
int tr_go(TrainController *controller);
int tr_stop(TrainController *controller);
int tr_set_speed(TrainController *controller, int tcain_number, int tcain_speed);
int tr_reverse(TrainController *controller, int train_number);
int tr_request_sensors(TrainController *controller, int max);
int tr_switch(TrainController *controller, int switch_number, char switch_direction);

// Delayed train set functions
int tr_schedule_delayed_switch(TrainController *controller);
int tr_schedule_delayed_reverse(TrainController *controller, int train_number);
