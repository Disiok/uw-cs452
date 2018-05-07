#include <ds.h>

/* 
 * Ring Buffer
 */

void rb_init(RingBuffer *ringBuffer) {
    ringBuffer->head = 0;
    ringBuffer->tail = 0;
    ringBuffer->size = 0;
    ringBuffer->maxSize = RING_BUFFER_SIZE;
    return;
}

void rb_set_max_size(RingBuffer *ringBuffer, int max_size) {
    if (max_size <= RING_BUFFER_SIZE) {
        ringBuffer->maxSize = max_size; 
    }
    return;
}

void rb_grow(RingBuffer *ringBuffer, char ch) {
    if (ringBuffer->size == ringBuffer->maxSize) {
        rb_shrink(ringBuffer); 
    }

    ringBuffer->buffer[ringBuffer->tail] = ch;
    ringBuffer->tail = (ringBuffer->tail + 1) % ringBuffer->maxSize;
    ringBuffer->size ++;
    return;
}

void rb_grow_int(RingBuffer *ringBuffer, int n) {
    rb_grow(ringBuffer, (char) n & 0xFF);
    rb_grow(ringBuffer, (char) (n>>8) & 0xFF);
    rb_grow(ringBuffer, (char) (n>>16) & 0xFF);
    rb_grow(ringBuffer, (char) (n>>24) & 0xFF);
    return;
} 

char rb_shrink(RingBuffer *ringBuffer) {
    char ch = ringBuffer->buffer[ringBuffer->head];
    ringBuffer->head = (ringBuffer->head + 1) % ringBuffer->maxSize;
    ringBuffer->size --;
    return ch;
}

int  rb_shrink_int(RingBuffer *ringBuffer) {
    int n_0 = rb_shrink(ringBuffer);
    int n_1 = rb_shrink(ringBuffer);
    int n_2 = rb_shrink(ringBuffer);
    int n_3 = rb_shrink(ringBuffer);

    int n =  n_0 + (n_1 << 8) + (n_2 << 16) + (n_3 << 24);
    return n;
}

char rb_peak(RingBuffer *ringBuffer, int ind) {
    return ringBuffer->buffer[(ringBuffer->head + ind) % ringBuffer->maxSize];
}

int rb_peak_int(RingBuffer *ringBuffer) {
    int n_0 = rb_peak(ringBuffer, 0);
    int n_1 = rb_peak(ringBuffer, 1);
    int n_2 = rb_peak(ringBuffer, 2);
    int n_3 = rb_peak(ringBuffer, 3);

    int n =  n_0 + (n_1 << 8) + (n_2 << 16) + (n_3 << 24);
    return n;
}

int rb_is_empty(RingBuffer *ringBuffer) {
    return ringBuffer->size == 0;

}

