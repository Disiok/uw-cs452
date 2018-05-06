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
    tc_render_static(&controller);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        // Update terminal channel
        st_poll(&st);

        // Update clock
        cl_poll(&clock);

        // Handle time
        tc_process_time(&controller, &clock);

        // Handle command
        if (tc_process_terminal_input(&controller)) {
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

