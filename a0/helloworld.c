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
    }
    bwputstr( COM2, "Done.\r\n"); 
    return 0;
}

