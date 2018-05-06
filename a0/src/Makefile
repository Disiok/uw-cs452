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

LDFLAGS = -init main -Map main.map -N  -T orex.ld -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -L../io/lib

all:  terminal.s time.s main.s main.elf

main.s: main.c
	$(XCC) -S $(CFLAGS) main.c

main.o: main.s
	$(AS) $(ASFLAGS) -o main.o main.s

time.s: time.c time.h
	$(XCC) -S $(CFLAGS) time.c

time.o: time.s
	$(AS) $(ASFLAGS) -o time.o time.s

terminal.s: terminal.c terminal.h
	$(XCC) -S $(CFLAGS) terminal.c

terminal.o: terminal.s
	$(AS) $(ASFLAGS) -o terminal.o terminal.s

main.elf: main.o time.o terminal.o
	$(LD) $(LDFLAGS) -o $@ main.o time.o terminal.o -lbwio -lio -lgcc

clean:
	-rm -f main.elf *.s *.o main.map
