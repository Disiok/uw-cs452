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

    FOREVER {
        putc(&channel, 'c');
        // put(&channel);
        if (DEBUG) {
            bwprintf( COM2, channel.writeBuffer->buffer); 
        }

        
    }
    /*
    setfifo( COM2, OFF ); 
    FOREVER {
        printf( COM2, "Looping.\n\r" ); 
        char command = getc( COM2 );
        printf( COM2, "Read char:" ); 
        putx( COM2, command );
        printf( COM2, "\n\r" ); 

        if (command == ENTER) {
            printf( COM2, "Breaking.\n\r" ); 
            break;
        }
    }
    */
    return 0;
}

