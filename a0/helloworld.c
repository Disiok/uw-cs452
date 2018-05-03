#include <bwio.h>
#include <ts7200.h>


#define FOREVER for( ; ; )
#define ENTER 0x0d


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
    bwsetfifo( COM2, OFF ); 
    bwprintf( COM2, "Hello world.\n\r" ); 
    FOREVER {
        bwprintf( COM2, "Looping.\n\r" ); 
        char command = bwgetc( COM2 );
        bwprintf( COM2, "Read char:" ); 
        bwputx( COM2, command );
        bwprintf( COM2, "\n\r" ); 

        if (command == ENTER) {
            bwprintf( COM2, "Breaking.\n\r" ); 
            break;
        }
    }
    return 0;
}

