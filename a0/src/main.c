#include <bwio.h>
#include <time.h>
#include <terminal.h>
#include <track.h>
#include <io.h>
#include <train.h>
#include <ts7200.h>

#define FOREVER for ( ; ; )
#define ENTER 0x0d
#define DEBUG_FLAG 1
#define DEBUG if (DEBUG_FLAG)

int main( int argc, char* argv[] ) {
    // Track
    Track track;
    ta_init(&track, 'a');

    // Clock setup
    Clock clock;
    cl_init(&clock);

    // Smart terminal setup
    SmartTerminal st;
    st_init(&st);
    st_render_static(&st, &track);

    // Train controller setup
    TrainController train_controller;
    tr_init(&train_controller, &clock, &track);

    // Terminal controller setup
    TerminalController controller;
    tc_init(&controller);

    // Main polling loop
    int iter = 0;
    int sum_loop_time_ms = 0;
    int max_loop_time_ms = 0;

    FOREVER {
        int start_time_ms = cl_get_time_ms(&clock);

        // Update clock, and clock display
        cl_poll(&clock, &st);
        
        // Update train channel, and sensor display
        tr_poll(&train_controller, &st);

        // Update terminal channel, and terminal display
        st_poll(&st);

        // Handle command, update command display, and send to train set
        if (tc_poll(&controller, &st, &train_controller)) {
            break; 
        }
        
        // Update clock, and clock display
        cl_poll(&clock, &st);
        int end_time_ms = cl_get_time_ms(&clock);
        int loop_time_ms = end_time_ms - start_time_ms;

        // Update loop times
        if (loop_time_ms > max_loop_time_ms) {
            max_loop_time_ms = loop_time_ms; 
        }
        sum_loop_time_ms += loop_time_ms; 

        if (iter % 1000 == 0) {
            st_save_cursor(&st);
            st_move_cursor(&st, 1, 50);
            printf(&(st.channel), "Mean: %d us",  sum_loop_time_ms);
            st_move_cursor(&st, 2, 50);
            printf(&(st.channel), "Max: %d ms",  max_loop_time_ms);
            st_restore_cursor(&st);

            sum_loop_time_ms = 0;
            max_loop_time_ms = 0;
        }
        
        iter ++;
    }
    return 0;
}

