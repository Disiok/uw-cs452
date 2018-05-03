#include <io.h>
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
    setfifo( COM2, OFF ); 
    printf( COM2, "Hello world.\n\r" ); 
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
    return 0;
}

