#pragma once

#include <track_data.h>

#define TRAIN_NUMBER_MAX 80
#define TRAIN_NUMBER_MIN 1

#define TRAIN_SWITCH_MAX 255
#define TRAIN_SWITCH_MIN 0

#define TRAIN_SENSOR_MAX 5
#define TRAIN_SENSOR_MIN 1

typedef struct Track Track;
struct Track {
    track_node nodes[TRACK_MAX];
    int switch_exist[TRAIN_SWITCH_MAX];
    const char *merge_names[TRAIN_SWITCH_MAX];
    const char *branch_names[TRAIN_SWITCH_MAX];
};


void ta_init(Track *track, char track_id);
