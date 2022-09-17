#ifndef BUFFER_H_
#define BUFFER_H_

#include "error.h"
#include "types.h"

typedef struct Buffer {
  usize len;

  Errors err;
} Buffer;

Buffer buffer_init();

void buffer_free(Buffer *buffer);

#endif
