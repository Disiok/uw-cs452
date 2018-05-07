#include <bwio.h>
#include <time.h>
#include <terminal.h>
#include <track_data.h>
#include <io.h>
#include <train.h>
#include <ts7200.h>

#define FOREVER for ( ; ; )
#define ENTER 0x0d
#define DEBUG_FLAG 1
#define DEBUG if (DEBUG_FLAG)

int main( int argc, char* argv[] ) {
    // Track
    track_node track[TRACK_MAX];
    init_tracka(track);

    // Clock setup
    Clock clock;
    cl_init(&clock);

    // Smart terminal setup
    SmartTerminal st;
    st_init(&st);
    st_render_static(&st, track);

    // Train controller setup
    TrainController train_controller;
    tr_init(&train_controller, &clock);

    // Terminal controller setup
    TerminalController controller;
    tc_init(&controller);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        // Update clock, and clock display
        cl_poll(&clock, &st);
        
        // Update train channel, and sensor display
        tr_poll(&train_controller, &st);

        // Update terminal channel, and terminal display
        st_poll(&st);

        // Handle command
        if (tc_poll(&controller, &st, &train_controller)) {
            break; 
        }
        
        /*
        long end_time_ms = cl_get_time_ms(&clock);
        loop_time_ms += end_time_ms - start_time_ms;
        if (iter % 100 == 0) {
            printf(&(st.channel), "%d ms to complete 100 loops.\r\n",  loop_time_ms);
            loop_time_ms = 0;
        }
        */
        
        iter ++;
    }
    return 0;
}

