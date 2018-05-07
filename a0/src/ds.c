#include <ds.h>

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
    ringBuffer->tail = (ringBuffer->tail + 1) % RING_BUFFER_SIZE;
    ringBuffer->size ++;
    return;
}

char rb_shrink(RingBuffer *ringBuffer) {
    char ch = ringBuffer->buffer[ringBuffer->head];
    ringBuffer->head = (ringBuffer->head + 1) % RING_BUFFER_SIZE;
    ringBuffer->size --;
    return ch;
}


char rb_peak(RingBuffer *ringBuffer, int ind) {
    return ringBuffer->buffer[ringBuffer->head + ind];
}

int rb_is_empty(RingBuffer *ringBuffer) {
    return ringBuffer->size == 0;

}

