/*
 * io.c - I/O routines for diagnosis
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */

#include <ts7200.h>
#include <io.h>

/* 
 * Ring Buffer
 */

void rb_init(RingBuffer *ringBuffer) {
    ringBuffer->head = 0;
    ringBuffer->tail = 0;
    ringBuffer->size = 0;
    return;
}

void rb_grow(RingBuffer *ringBuffer, char ch) {
    ringBuffer->buffer[ringBuffer->tail] = ch;
    ringBuffer->tail = (ringBuffer->tail + 1) % BUFFER_SIZE;
    ringBuffer->size ++;
    return;
}

char rb_shrink(RingBuffer *ringBuffer) {
    char ch = ringBuffer->buffer[ringBuffer->head];
    ringBuffer->head = (ringBuffer->head + 1) % BUFFER_SIZE;
    ringBuffer->size --;
    return ch;
}

int rb_is_empty(RingBuffer *ringBuffer) {
    return ringBuffer->size == 0;

}

/* 
 * Buffered Channel
 */

void bc_init(BufferedChannel *channel, int id) {
    channel->id = id;
    rb_init(&channel->readBuffer);
    rb_init(&channel->writeBuffer);

    // Default settings
    setfifo(channel, OFF);
    return;
}

/*
 * Smart Terminal
 */

void st_init(SmartTerminal *st, int id) {
    bc_init(&(st->channel), id);
    st->size = 0;
}

void st_poll(SmartTerminal *st) {
    // Write from write buffer to URT
    if (!rb_is_empty(&(st->channel.writeBuffer))) {
        put(&(st->channel));
    }

    // Read from URT to read buffer
    get(&(st->channel));
}

int st_process_terminal_input(SmartTerminal *st, TerminalController *controller) {
    BufferedChannel *channel =  &(st->channel);
    if (!rb_is_empty(&(channel->readBuffer))) {
        char ch = getc(channel);

        // Echo visible input from terminal
        if (ch >= CHAR_VISIBLE_START && ch <= CHAR_VISIBLE_END) {
            putc(channel, ch);

            // Add to command buffer
            // TODO(sdsuo): Check for overflow here
            st->commandBuffer[st->size] = ch;
            st->size ++;
        } else if (ch == CHAR_ENTER) {
            tc_update_command(controller, st->commandBuffer);

            // NOTE(sdsuo): Wipe out the command buffer
            st->size = 0;
        } else {
            // TODO(sdsuo): Handle other cases (e.g. backspace)
        }
    } 
    return 0;
}

void st_save_cursor(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B""7");
}

void st_restore_cursor(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B""8");
}

void st_move_cursor_top_left(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[H");
}

void st_move_cursour(SmartTerminal *st, int vertical, int horizontal) {
    printf(&(st->channel), "\x1B[%d;%dH", vertical, horizontal);
}

void st_clear_line(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[2K");
}

void st_clear_screen(SmartTerminal *st) {
    putstr(&(st->channel), "\x1B[2J");
}

/*
 * The UARTs are initialized by RedBoot to the following state
 * 	115,200 bps
 * 	8 bits
 * 	no parity
 * 	fifos enabled
 */
int setfifo( BufferedChannel *channel, int state ) {
	int *line, buf;
	switch( channel->id ) {
	case COM1:
		line = (int *)( UART1_BASE + UART_LCRH_OFFSET );
	        break;
	case COM2:
	        line = (int *)( UART2_BASE + UART_LCRH_OFFSET );
	        break;
	default:
	        return -1;
	        break;
	}
	buf = *line;
	buf = state ? buf | FEN_MASK : buf & ~FEN_MASK;
	*line = buf;
	return 0;
}

int setspeed( BufferedChannel *channel, int speed ) {
	int *high, *low;
	switch( channel->id ) {
	case COM1:
		high = (int *)( UART1_BASE + UART_LCRM_OFFSET );
		low = (int *)( UART1_BASE + UART_LCRL_OFFSET );
	        break;
	case COM2:
		high = (int *)( UART2_BASE + UART_LCRM_OFFSET );
		low = (int *)( UART2_BASE + UART_LCRL_OFFSET );
	        break;
	default:
	        return -1;
	        break;
	}
	switch( speed ) {
	case 115200:
		*high = 0x0;
		*low = 0x3;
		return 0;
	case 2400:
		*high = 0x0;
		*low = 0x90;
		return 0;
	default:
		return -1;
	}
}

void put( BufferedChannel *channel ) {
	int *flags, *data;
	switch( channel->id ) {
	case COM1:
		flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
		data = (int *)( UART1_BASE + UART_DATA_OFFSET );
		break;
	case COM2:
		flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
		data = (int *)( UART2_BASE + UART_DATA_OFFSET );
		break;
	default:
		break;
	}
	if( !(*flags & TXFF_MASK) ) {
        *data = rb_shrink(&(channel->writeBuffer));
    }
    return;
}

int putc( BufferedChannel *channel, char c ) {
    rb_grow(&(channel->writeBuffer), c);
    return 0;
}

char c2x( char ch ) {
	if ( (ch <= 9) ) return '0' + ch;
	return 'a' + ch - 10;
}

int putx( BufferedChannel *channel, char c ) {
	char chh, chl;

	chh = c2x( c / 16 );
	chl = c2x( c % 16 );
	putc( channel, chh );
	return putc( channel, chl );
}

int putr( BufferedChannel *channel, unsigned int reg ) {
	int byte;
	char *ch = (char *) &reg;

	for( byte = 3; byte >= 0; byte-- ) putx( channel, ch[byte] );
	return putc( channel, ' ' );
}

int putstr( BufferedChannel *channel, char *str ) {
	while( *str ) {
		if( putc( channel, *str ) < 0 ) return -1;
		str++;
	}
	return 0;
}

void putw( BufferedChannel *channel, int n, char fc, char *bf ) {
	char ch;
	char *p = bf;

	while( *p++ && n > 0 ) n--;
	while( n-- > 0 ) putc( channel, fc );
	while( ( ch = *bf++ ) ) putc( channel, ch );
}

int getc( BufferedChannel *channel) {
    char ch = rb_shrink(&(channel->readBuffer));
    return ch;
}

void get( BufferedChannel *channel ) {
	int *flags, *data;

	switch( channel->id ) {
	case COM1:
		flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
		data = (int *)( UART1_BASE + UART_DATA_OFFSET );
		break;
	case COM2:
		flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
		data = (int *)( UART2_BASE + UART_DATA_OFFSET );
		break;
	default:
		break;
	}

	if( (*flags & RXFF_MASK) ) {
        rb_grow(&(channel->readBuffer), (char) *data);
    }

    return;
}

int a2d( char ch ) {
	if( ch >= '0' && ch <= '9' ) return ch - '0';
	if( ch >= 'a' && ch <= 'f' ) return ch - 'a' + 10;
	if( ch >= 'A' && ch <= 'F' ) return ch - 'A' + 10;
	return -1;
}

char a2i( char ch, char **src, int base, int *nump ) {
	int num, digit;
	char *p;

	p = *src; num = 0;
	while( ( digit = a2d( ch ) ) >= 0 ) {
		if ( digit > base ) break;
		num = num*base + digit;
		ch = *p++;
	}
	*src = p; *nump = num;
	return ch;
}

void ui2a( unsigned int num, unsigned int base, char *bf ) {
	int n = 0;
	int dgt;
	unsigned int d = 1;
	
	while( (num / d) >= base ) d *= base;
	while( d != 0 ) {
		dgt = num / d;
		num %= d;
		d /= base;
		if( n || dgt > 0 || d == 0 ) {
			*bf++ = dgt + ( dgt < 10 ? '0' : 'a' - 10 );
			++n;
		}
	}
	*bf = 0;
}

void i2a( int num, char *bf ) {
	if( num < 0 ) {
		num = -num;
		*bf++ = '-';
	}
	ui2a( num, 10, bf );
}

void format ( BufferedChannel *channel, char *fmt, va_list va ) {
	char bf[12];
	char ch, lz;
	int w;

	
	while ( ( ch = *(fmt++) ) ) {
		if ( ch != '%' )
			putc( channel, ch );
		else {
			lz = 0; w = 0;
			ch = *(fmt++);
			switch ( ch ) {
			case '0':
				lz = 1; ch = *(fmt++);
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				ch = a2i( ch, &fmt, 10, &w );
				break;
			}
			switch( ch ) {
			case 0: return;
			case 'c':
				putc( channel, va_arg( va, char ) );
				break;
			case 's':
				putw( channel, w, 0, va_arg( va, char* ) );
				break;
			case 'u':
				ui2a( va_arg( va, unsigned int ), 10, bf );
				putw( channel, w, lz, bf );
				break;
			case 'd':
				i2a( va_arg( va, int ), bf );
				putw( channel, w, lz, bf );
				break;
			case 'x':
				ui2a( va_arg( va, unsigned int ), 16, bf );
				putw( channel, w, lz, bf );
				break;
			case '%':
				putc( channel, ch );
				break;
			}
		}
	}
}

void printf( BufferedChannel *channel, char *fmt, ... ) {
        va_list va;

        va_start(va,fmt);
        format( channel, fmt, va );
        va_end(va);
}

