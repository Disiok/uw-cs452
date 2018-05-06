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

    // Smart terminal setup
    SmartTerminal st;
    st_init(&st, COM2);

    BufferedChannel train_channel;
    bc_init(&train_channel, COM1);
    tr_init_protocol(&train_channel);

    // Clock setup
    Clock clock;
    cl_init(&clock);

    // Terminal controller
    TerminalController controller;
    tc_init(&controller, &st);

    // Start of execution
    tc_render_static(&controller, track);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;

    long prev_on = 0;
    long prev_off = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        /*
        if (start_time_ms % 5000 == 0 && start_time_ms != prev_on) {
            tr_sw(&train_channel, 4, 's');
            tr_sw(&train_channel, 4, 'c');
            prev_on = start_time_ms;
        }

        if (start_time_ms % 5150 == 0 && start_time_ms != prev_off) {
            putc(&train_channel, TRAIN_SWITCH_OFF);
            prev_off = start_time_ms;
        }
        */

        // Update train channel
        bc_poll(&train_channel);

        // Update terminal channel
        st_poll(&st);

        // Update clock
        cl_poll(&clock);

        // Handle time
        tc_process_time(&controller, &clock);

        // Handle command
        if (st_process_terminal_input(&st, &controller, &train_channel)) {
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

