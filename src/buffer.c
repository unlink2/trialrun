#include "buffer.h"
#include "macros.h"
#include <stdlib.h>
#include <string.h>

Buffer buffer_init(usize len) {
  Buffer buffer;
  memset(&buffer, 0, sizeof(Buffer));

  buffer.str.raw = malloc(len);
  buffer.str.len = 0;
  memset(buffer.str.raw, 0, len);

  buffer.space = len;
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

#ifdef TEST

#include "macros.h"

void test_buffer_resize(void **state) {
  Buffer b = buffer_init(4);

  for (int i = 0; i < 10; i++) {
    buffer_write(&b, '1' + i);
  }

  assert_true(trstr_eq_raw(b.str, "12345789"));

  buffer_free(&b);
}

#endif
