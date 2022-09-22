#include "buffer.h"
#include "macros.h"
#include <stdlib.h>
#include <string.h>

StrBuffer strbuffer_init(usize len) {
  StrBuffer buffer;
  memset(&buffer, 0, sizeof(StrBuffer));

  buffer.str.raw = malloc(len);
  buffer.str.len = 0;
  memset(buffer.str.raw, 0, len);

  buffer.cap = len;
  return buffer;
}

void strbuffer_resize(StrBuffer *buffer, usize len) {
  char *old = buffer->str.raw;
  usize old_len = buffer->cap;

  // new buffeer
  buffer->str.raw = malloc(len);
  memset(buffer->str.raw, 0, len);
  buffer->cap = len;

  if (old) {
    memcpy(buffer->str.raw, old, MIN(old_len, len));
    free(old);
  }
}

usize strbuffer_next_len(StrBuffer *buffer) { return buffer->cap * 2; }

bool strbuffer_full(StrBuffer *buffer) {
  return buffer->str.len >= buffer->cap - 1;
}

void strbuffer_write(StrBuffer *buffer, char c) {
  if (strbuffer_full(buffer)) {
    strbuffer_resize(buffer, strbuffer_next_len(buffer));
  }
  buffer->str.raw[buffer->str.len++] = c;
}

void strbuffer_append_raw(StrBuffer *buffer, char *s) {
  while (*s != '\0') {
    strbuffer_write(buffer, *s);
    s++;
  }
}

void strbuffer_clear(StrBuffer *buffer) { buffer->str.len = 0; }

void strbuffer_free(StrBuffer *buffer) {
  if (buffer->str.raw) {
    free(buffer->str.raw);
  }
}

#ifdef TEST

#include "macros.h"

void test_strbuffer_resize(void **state) {
  StrBuffer b = strbuffer_init(4);

  assert_int_equal(4, b.cap);
  assert_int_equal(0, b.str.len);

  for (int i = 0; i < 10; i++) {
    strbuffer_write(&b, '1' + i);
  }

  // should have resized twice!
  assert_int_equal(16, b.cap);
  assert_int_equal(9, b.str.len);
  assert_true(str_eq_raw(b.str, "12345789"));

  strbuffer_free(&b);
}

#endif
