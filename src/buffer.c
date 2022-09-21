#include "buffer.h"
#include "macros.h"
#include <stdlib.h>
#include <string.h>

Buffer buffer_init(usize len) {
  Buffer buffer;
  memset(&buffer, 0, sizeof(Buffer));
  buffer.str.raw = malloc(len);
  memset(buffer.str.raw, 0, len);
  buffer.str.len = len;
  return buffer;
}

void buffer_resize(Buffer *buffer, usize len) {
  char *old = buffer->str.raw;
  usize old_len = buffer->space;

  // new buffeer
  buffer->str.raw = malloc(len);
  memset(buffer->str.raw, 0, len);
  buffer->space = len;

  if (old) {
    memcpy(buffer->str.raw, old, MIN(old_len, len));
    free(old);
  }
}

usize buffer_next_len(Buffer *buffer) { return buffer->space * 2; }

bool buffer_full(Buffer *buffer) {
  return buffer->str.len >= buffer->space - 1;
}

void buffer_write(Buffer *buffer, char c) {
  if (buffer_full(buffer)) {
    buffer_resize(buffer, buffer_next_len(buffer));
  }
  buffer->str.raw[buffer->str.len++] = c;
}

void buffer_free(Buffer *buffer) {
  if (buffer->str.raw) {
    free(buffer->str.raw);
  }
}
