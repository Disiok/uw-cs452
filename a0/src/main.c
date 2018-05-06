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

    BufferedChannel channel;
    bc_init(&channel, COM1);

    Clock clock;
    cl_init(&clock);

    TerminalController controller;
    tc_init(&controller, &st);

    // Start of execution
    // tc_render_static(&controller);

    // Main polling loop
    int iter = 0;
    int loop_time_ms = 0;


    // Train communication setup
    
    /*
     * Baud rate = 2400
     */
    bwsetspeed(COM1, 2400);
    int speed_high = *(int *)(UART1_BASE + UART_LCRM_OFFSET);
    int speed_low = *(int *)(UART1_BASE + UART_LCRL_OFFSET);

    /*
     * Start bits (if requested by computer) = 1
     * Stop bits = 2
     * Parity = None
     * Word size = 8 bits
     */
    int *line = (int *)( UART1_BASE + UART_LCRH_OFFSET );

	int buf = *line;
    // FIFO off
	buf =  buf & ~FEN_MASK;
    // parity disable
	buf =  buf & ~PEN_MASK & ~STP2_MASK;
    // stop bits 2
    buf = buf | STP2_MASK;
    // word size 8
    buf = buf | WLEN_MASK;
	*line = buf;

    bwprintf(COM2, "What the fuck\r\n");
    bwprintf(COM2, "%x\r\n", buf);
    bwprintf(COM2, "%x\r\n", speed_high);
    bwprintf(COM2, "%x\r\n", speed_low);


    FOREVER {
        bwputc(COM1, 0x60);

        long start_time_ms = cl_get_time_ms(&clock);

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

