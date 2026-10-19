#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Buffer {
  uint8_t* data;
  int size;
  int head;
  int tail;

} Buffer;

Buffer* buffer_new(int size);

void buffer_free(Buffer* rb);

uint8_t* buffer_peak_head(Buffer* rb, int* size);

int buffer_push_tail(Buffer* rb, const uint8_t* data, int size);

int buffer_push_tail_with_prefix(Buffer* rb, const uint8_t* prefix,
                                 int prefix_size, const uint8_t* data,
                                 int size);

void buffer_pop_head(Buffer* rb);

void buffer_clear(Buffer* rb);

int buffer_get_used(const Buffer* rb);

int buffer_get_capacity(const Buffer* rb);

#endif  // BUFFER_H_
