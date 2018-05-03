/*
 * io.h
 */

typedef char *va_list;

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

int setfifo( int channel, int state );

int setspeed( int channel, int speed );

int putc( int channel, char c );

int getc( int channel );

int putx( int channel, char c );

int putstr( int channel, char *str );

int putr( int channel, unsigned int reg );

void putw( int channel, int n, char fc, char *bf );

void printf( int channel, char *format, ... );
