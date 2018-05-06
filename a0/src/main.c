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
    // Smart terminal setup
    SmartTerminal st;
    st_init(&st, COM2);

    BufferedChannel train_channel;
    bc_init(&train_channel, COM1);

    // Train communication setup
    // Set Baud rate = 2400
    setspeed(&train_channel, 2400);

    /*
     * Set control
     *
     * Start bits (if requested by computer) = 1
     * Stop bits = 2
     * Parity = None
     * Word size = 8 bits
     */
    setnoparity(&train_channel);
    set2stopbits(&train_channel);
    set8wordsize(&train_channel);
    setfifo(&train_channel, 0);

    // Clock setup
    Clock clock;
    cl_init(&clock);

    // Terminal controller
    TerminalController controller;
    tc_init(&controller, &st);

    // Start of execution
    // tc_render_static(&controller);

    int speed_high = *(int *)(UART1_BASE + UART_LCRM_OFFSET);
    int speed_low = *(int *)(UART1_BASE + UART_LCRL_OFFSET);
    int control = *(int *)( UART1_BASE + UART_LCRH_OFFSET );

    bwprintf(COM2, "Control bits: \r\n");
    bwprintf(COM2, "%x\r\n", control);
    bwprintf(COM2, "%x\r\n", speed_high);
    bwprintf(COM2, "%x\r\n", speed_low);

    putc(&train_channel, 0x60);

    putc(&train_channel, 0x61);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;

    FOREVER {
        long start_time_ms = cl_get_time_ms(&clock);

        // Update train channel
        bc_poll(&train_channel);

        // Update terminal channel
        st_poll(&st);

        // Update clock
        cl_poll(&clock);

        /*
        // Handle time
        tc_process_time(&controller, &clock);

        // Handle command
        if (tc_process_terminal_input(&controller)) {
            break; 
        }
        */
        
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

