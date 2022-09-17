#include "buffer.h"
#include <string.h>

Buffer buffer_init() {
  Buffer buffer;
  memset(&buffer, 0, sizeof(Buffer));
  return buffer;
}

void buffer_free(Buffer *buffer) {}
