#include <bwio.h>
#include <time.h>
#include <terminal.h>
#include <io.h>
#include <ts7200.h>

#define FOREVER for ( ; ; )
#define ENTER 0x0d
#define DEBUG_FLAG 1
#define DEBUG if (DEBUG_FLAG)

int tr( int train_number, int train_speed ) {
    return 0;
}

int rv( int train_number ) {
    return 0;
}

int sw( int switch_number, int switch_direction) {
    return 0;
}


int main( int argc, char* argv[] ) {
    BufferedChannel channel;
    bc_init(&channel, COM2);

    Clock clock;
    cl_init(&clock);

    TerminalController controller;
    tc_init(&controller);

    DEBUG {
        bwprintf( COM2, "Hello world.\n\r\0" ); 
    }

    int iter = 0;
    int prev_clock_value = 2;
    int loop_time_ms = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        // Write from write buffer to URT
        if (!rb_is_empty(&(channel.writeBuffer))) {
            put(&channel);
        }

        // Read from URT to read buffer
        get(&channel);

        // Update clock
        cl_tick(&clock);

        // Handle command
        // tc_process_terminal_input(&controller, &channel);

        // Handle time
        if (iter % 1000 == 0) {
            tc_process_time(&controller, &clock, &channel);
        }
        
        /*
        long end_time_ms = cl_get_time_ms(&clock);
        loop_time_ms += end_time_ms - start_time_ms;
        if (iter % 100 == 0) {
            printf(&channel, "%d ms to complete 100 loops.\r\n",  loop_time_ms);
            loop_time_ms = 0;
        }
        */
        
        iter ++;
    }
    
    DEBUG {
        bwputstr( COM2, "Done.\r\n"); 
    }
    return 0;
}

