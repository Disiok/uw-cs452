/*
 * io.h
 */

#ifndef _VA_LIST_
#define _VA_LIST_
typedef char *va_list;
#endif 

#define __va_argsiz(t)	\
		(((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)	((void)0)

#define va_arg(ap, t)	\
		 (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

#define COM1	0
#define COM2	1

#define ON	1
#define	OFF	0

#define BUFFER_SIZE 32

typedef struct {
    int head;
    int tail;
    int size;
    char buffer[BUFFER_SIZE];
} RingBuffer;

typedef struct {
    int id;
    RingBuffer *readBuffer;
    RingBuffer *writeBuffer;
    
} BufferedChannel;

void init( RingBuffer *channel);

void grow( RingBuffer *channel, char ch);

char shrink( RingBuffer *channel);

int setfifo( BufferedChannel *channel, int state );

int setspeed( BufferedChannel *channel, int speed );

void put( BufferedChannel *channel );

void get( BufferedChannel *channel );

int putc( BufferedChannel *channel, char c );

int getc( BufferedChannel *channel );

int putx( BufferedChannel *channel, char c );

int putstr( BufferedChannel *channel, char *str );

int putr( BufferedChannel *channel, unsigned int reg );

void putw( BufferedChannel *channel, int n, char fc, char *bf );

void printf( BufferedChannel *channel, char *format, ... );
