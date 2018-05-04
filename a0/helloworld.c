#include <bwio.h>
#include <io.h>
#include <ts7200.h>

#define FOREVER for( ; ; )
#define ENTER 0x0d
#define DEBUG 1

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
    if (DEBUG) {
        bwprintf( COM2, "Hello world.\n\r" ); 
    }

    RingBuffer readBuffer = {"", 0, 0};
    RingBuffer writeBuffer = {"", 0, 0};
    BufferedChannel channel = {COM2, &readBuffer, &writeBuffer};

    putc(&channel, 'c');
    if (DEBUG) {
        bwprintf( COM2, channel.writeBuffer->buffer); 
    }

    FOREVER {
        put(&channel);
        get(&channel);
    }
    return 0;
}

