
#pragma once 

#define RING_BUFFER_SIZE 2048

typedef struct {
    int head;
    int tail;
    int size;
    char buffer[RING_BUFFER_SIZE];
} RingBuffer;

void rb_init(RingBuffer *channel);

void rb_grow(RingBuffer *channel, char ch);

char rb_shrink(RingBuffer *channel);

char rb_peak(RingBuffer *ringBuffer, int ind);

int rb_is_empty(RingBuffer *channel);

