
#pragma once 

#define RING_BUFFER_SIZE 2048

typedef struct {
    int head;
    int tail;
    int size;
    char buffer[RING_BUFFER_SIZE];
    int maxSize;
} RingBuffer;

void rb_init(RingBuffer *channel);

void rb_set_max_size(RingBuffer *ringBuffer, int max_size);

void rb_grow(RingBuffer *channel, char ch);

void rb_grow_int(RingBuffer *channel, int n);

char rb_shrink(RingBuffer *channel);

int rb_shrink_int(RingBuffer *channel);

char rb_peak(RingBuffer *ringBuffer, int ind);

int rb_peak_int(RingBuffer *ringBuffer);

int rb_is_empty(RingBuffer *channel);

