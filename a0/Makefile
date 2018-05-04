#
# Makefile for busy-wait IO tests
#
XCC     = gcc
AS	= as
LD      = ld
CFLAGS  = -c -fPIC -Wall -I. -I../io/include -mcpu=arm920t -msoft-float
# -g: include hooks for gdb
# -c: only compile
# -mcpu=arm920t: generate code for the 920t architecture
# -fpic: emit position-independent code
# -Wall: report all warnings

ASFLAGS	= -mcpu=arm920t -mapcs-32
# -mapcs: always generate a complete stack frame

LDFLAGS = -init main -Map helloworld.map -N  -T orex.ld -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -L../io/lib

all:  time.s helloworld.s helloworld.elf

helloworld.s: helloworld.c helloworld.h
	$(XCC) -S $(CFLAGS) helloworld.c

helloworld.o: helloworld.s
	$(AS) $(ASFLAGS) -o helloworld.o helloworld.s

time.s: time.c time.h
	$(XCC) -S $(CFLAGS) time.c

time.o: time.s
	$(AS) $(ASFLAGS) -o time.o time.s

helloworld.elf: helloworld.o time.o
	$(LD) $(LDFLAGS) -o $@ helloworld.o time.o -lbwio -lio -lgcc

clean:
	-rm -f helloworld.elf *.s *.o helloworld.map
