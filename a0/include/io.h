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
#define COMMAND_BUFFER_SIZE 64

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

typedef struct TerminalController;

typedef struct {
    BufferedChannel channel;
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int size;
} SmartTerminal;

void st_init(SmartTerminal *st, int id);
void st_poll(SmartTerminal *st);
int st_process_terminal_input(SmartTerminal *st, TerminalController *controller);
void st_save_cursor(SmartTerminal *st);
void st_restore_cursor(SmartTerminal *st);
void st_move_cursor(SmartTerminal *st, int vertical, int horizontal);
void st_move_cursor_top_left(SmartTerminal *st);
void st_clear_line(SmartTerminal *st);
void st_clear_screen(SmartTerminal *st);

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
