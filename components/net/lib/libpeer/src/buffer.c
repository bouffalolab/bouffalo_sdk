#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "utils.h"

#define BUFFER_WRAP_MARKER (-1)
#define BUFFER_ALIGNMENT 4

static int buffer_record_size(int payload_size) {
  if (payload_size <= 0 ||
      payload_size >
          INT_MAX - (int)sizeof(int) - (BUFFER_ALIGNMENT - 1)) {
    return -1;
  }
  return ALIGN32(payload_size + (int)sizeof(int));
}

static void buffer_normalize_head(Buffer* rb) {
  if (rb->head != rb->tail &&
      *((int*)(rb->data + rb->head)) == BUFFER_WRAP_MARKER) {
    rb->head = 0;
  }
}

Buffer* buffer_new(int size) {
  Buffer* rb;

  size &= ~(BUFFER_ALIGNMENT - 1);
  if (size < BUFFER_ALIGNMENT * 2) {
    return NULL;
  }

  rb = (Buffer*)calloc(1, sizeof(Buffer));
  if (!rb) {
    return NULL;
  }

  rb->data = (uint8_t*)malloc((size_t)size);
  if (!rb->data) {
    free(rb);
    return NULL;
  }

  rb->size = size;
  return rb;
}

void buffer_clear(Buffer* rb) {
  if (rb) {
    rb->head = 0;
    rb->tail = 0;
  }
}

void buffer_free(Buffer* rb) {
  if (rb) {
    free(rb->data);
    free(rb);
  }
}

int buffer_push_tail(Buffer* rb, const uint8_t* data, int size) {
  return buffer_push_tail_with_prefix(rb, NULL, 0, data, size);
}

int buffer_push_tail_with_prefix(Buffer* rb, const uint8_t* prefix,
                                 int prefix_size, const uint8_t* data,
                                 int size) {
  int payload_size;
  int record_size;

  if (!rb || !data || size <= 0 || prefix_size < 0 ||
      (prefix_size > 0 && !prefix) || prefix_size > INT_MAX - size) {
    return -1;
  }

  payload_size = prefix_size + size;
  record_size = buffer_record_size(payload_size);
  if (record_size <= 0 || record_size > rb->size - BUFFER_ALIGNMENT) {
    LOGE("buffer record is too large");
    return -1;
  }

  if (rb->tail >= rb->head) {
    int end_space = rb->size - rb->tail;

    if (end_space < record_size) {
      if (rb->head < record_size + BUFFER_ALIGNMENT ||
          end_space < (int)sizeof(int)) {
        LOGE("no enough space");
        return -1;
      }
      *((int*)(rb->data + rb->tail)) = BUFFER_WRAP_MARKER;
      rb->tail = 0;
    } else if (rb->head == 0 &&
               end_space < record_size + BUFFER_ALIGNMENT) {
      LOGE("no enough space");
      return -1;
    }
  } else if (rb->head - rb->tail < record_size + BUFFER_ALIGNMENT) {
    LOGE("no enough space");
    return -1;
  }

  *((int*)(rb->data + rb->tail)) = payload_size;
  if (prefix_size > 0) {
    memcpy(rb->data + rb->tail + sizeof(int), prefix, (size_t)prefix_size);
  }
  memcpy(rb->data + rb->tail + sizeof(int) + prefix_size, data,
         (size_t)size);
  rb->tail += record_size;
  if (rb->tail == rb->size) {
    rb->tail = 0;
  }

  return size;
}

uint8_t* buffer_peak_head(Buffer* rb, int* size) {
  int record_size;

  if (!rb || !size || rb->head == rb->tail) {
    return NULL;
  }

  buffer_normalize_head(rb);
  if (rb->head == rb->tail) {
    return NULL;
  }

  *size = *((int*)(rb->data + rb->head));
  record_size = buffer_record_size(*size);
  if (*size <= 0 || record_size <= 0 ||
      record_size > rb->size - rb->head) {
    LOGE("corrupt buffer record");
    buffer_clear(rb);
    return NULL;
  }

  return rb->data + rb->head + sizeof(int);
}

void buffer_pop_head(Buffer* rb) {
  int size;
  int record_size;

  if (!rb || rb->head == rb->tail) {
    return;
  }

  buffer_normalize_head(rb);
  if (rb->head == rb->tail) {
    return;
  }

  size = *((int*)(rb->data + rb->head));
  record_size = buffer_record_size(size);
  if (size <= 0 || record_size <= 0 ||
      record_size > rb->size - rb->head) {
    LOGE("corrupt buffer record");
    buffer_clear(rb);
    return;
  }

  rb->head += record_size;
  if (rb->head == rb->size) {
    rb->head = 0;
  }
}

int buffer_get_used(const Buffer* rb) {
  int cursor;
  int used = 0;

  if (!rb || rb->head == rb->tail) {
    return 0;
  }

  cursor = rb->head;
  while (cursor != rb->tail && used < rb->size) {
    int payload_size = *((int*)(rb->data + cursor));
    int record_size;

    if (payload_size == BUFFER_WRAP_MARKER) {
      used += rb->size - cursor;
      cursor = 0;
      continue;
    }

    record_size = buffer_record_size(payload_size);
    if (record_size <= 0 || record_size > rb->size - cursor) {
      return -1;
    }
    used += record_size;
    cursor += record_size;
    if (cursor == rb->size) {
      cursor = 0;
    }
  }

  return cursor == rb->tail ? used : -1;
}

int buffer_get_capacity(const Buffer* rb) {
  return rb ? rb->size - BUFFER_ALIGNMENT : 0;
}
