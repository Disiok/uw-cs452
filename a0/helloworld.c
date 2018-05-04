#include <bwio.h>
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
    RingBuffer readBuffer;
    init(&readBuffer);
    RingBuffer writeBuffer;
    init(&writeBuffer);
    BufferedChannel channel = {COM2, &readBuffer, &writeBuffer};

    setfifo(&channel, OFF);

    DEBUG {
        printf( &channel, "Hello world.\n\r\0" ); 
    }

    // get clock registers
    int *clock_load_addr = (int *) (TIMER3_BASE + LDR_OFFSET);
    int *clock_value_addr = (int *) (TIMER3_BASE + VAL_OFFSET);
    int *clock_control_addr = (int *) (TIMER3_BASE + CRTL_OFFSET);

    // load starting value into load register for clock
    *clock_load_addr = 2;

    // flip enable bit to start clock, set to periodic mode
    int buf = *clock_control_addr;
    buf = buf | ENABLE_MASK | MODE_MASK & ~CLKSEL_MASK;
    *clock_control_addr = buf;

    int prev = 2;
    int ticks = 0;

    FOREVER {
        // Write from write buffer to URT
        if (!is_empty(&writeBuffer)) {
            put(&channel);
        }

        // Read from URT to read buffer
        get(&channel);

        // Echo input from terminal
        if (!is_empty(&readBuffer)) {
            char ch = getc(&channel);
            putc(&channel, ch);
        }

        // update tick counter
        int clock_value =  *clock_value_addr;
        if (clock_value > prev) {
            ticks++;
            printf(&channel, "%d\r\n", ticks);
        }
        prev = clock_value;
    }
    bwputstr( COM2, "Done.\r\n"); 
    return 0;
}

