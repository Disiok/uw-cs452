#include <track.h>



void ta_init(Track *track, char track_id) {
    if (track_id == 'a') {
        init_tracka(track->nodes);
    } else if (track_id == 'b') {
        init_trackb(track->nodes);
    }

    int i;
    int n_switches = 0;
    for (i = 0; i < TRAIN_SWITCH_MAX; i++) {
        track->switch_exist[i] = -1;
    }

    track_node *nodes = track->nodes;
    for (i = 0; i < TRACK_MAX; i++) {
        if (nodes[i].type == NODE_BRANCH) {
           track->branch_names[nodes[i].num] = nodes[i].name;

           if (track->switch_exist[nodes[i].num] == -1) {
               track->switch_exist[nodes[i].num]= n_switches;
               n_switches ++;
           } 
        } else if (nodes[i].type == NODE_MERGE) {
           track->merge_names[nodes[i].num] = nodes[i].name;

           if (track->switch_exist[nodes[i].num] == -1) {
               track->switch_exist[nodes[i].num]= n_switches;
               n_switches ++;
           } 
        }
    }

}
