/*
 * io.h
 */

#pragma once 

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

void rb_init(RingBuffer *channel);

void rb_grow(RingBuffer *channel, char ch);

char rb_shrink(RingBuffer *channel);

int rb_is_empty(RingBuffer *channel);

typedef struct {
    int id;
    RingBuffer readBuffer;
    RingBuffer writeBuffer;
    
} BufferedChannel;

void bc_init( BufferedChannel *channel, int id);


/*
 * Smart Terminal: wrapper on gtk for moving cursors around
 *
 * This contains logic for:
 * 1. Basic functions dealing with cursors
 */

void st_save(BufferedChannel *channel);
void st_restore(BufferedChannel *channel);
void st_move(BufferedChannel *channel, int vertical, int horizontal);
void st_move_top_left(BufferedChannel *channel);
void st_clear_line(BufferedChannel *channel);

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
