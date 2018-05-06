#include <bwio.h>
#include <time.h>
#include <terminal.h>
#include <io.h>
#include <ts7200.h>

#define FOREVER for ( ; ; )
#define ENTER 0x0d
#define DEBUG_FLAG 1
#define DEBUG if (DEBUG_FLAG)

int main( int argc, char* argv[] ) {
    // Objects
    SmartTerminal st;
    st_init(&st, COM2);

    Clock clock;
    cl_init(&clock);

    TerminalController controller;
    tc_init(&controller, &st);

    // Start of execution
    DEBUG {
        bwprintf( COM2, "Hello world.\n\r\0" ); 
    }

    st_clear_screen(&st);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        st_poll(&st);

        // Update clock
        cl_tick(&clock);

        // Handle command
        tc_process_terminal_input(&controller);

        // Handle time
        tc_process_time(&controller, &clock);
        
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
    
    // Clean up
    DEBUG {
        bwputstr( COM2, "Done.\r\n"); 
    }
    return 0;
}

