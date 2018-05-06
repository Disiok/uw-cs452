#
# Makefile for busy-wait IO tests
#
XCC     = gcc
AS		= as
LD      = ld
CFLAGS  = -c -fPIC -Wall -I. -I./include -mcpu=arm920t -msoft-float
# -g: include hooks for gdb
# -c: only compile
# -mcpu=arm920t: generate code for the 920t architecture
# -fpic: emit position-independent code
# -Wall: report all warnings

ASFLAGS	= -mcpu=arm920t -mapcs-32
# -mapcs: always generate a complete stack frame

LDFLAGS = -init main -Map main.map -N  -T orex.ld -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2


LIB := -lgcc
TARGET := build/main.elf
SOURCES := $(shell find src -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
ASMS := $(patsubst src/%,build/%,$(SOURCES:.c=.s))


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIB)

build/%.o: build/%.s
	$(AS) $(ASFLAGS) -o $@ $<

build/%.s: src/%.c
	$(XCC) -S $(CFLAGS) -o $@ $<

clean:
	-rm -f build/main.elf build/*.s build/*.o build/main.map
