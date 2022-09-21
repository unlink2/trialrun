#ifndef BUFFER_H_
#define BUFFER_H_

#include "error.h"
#include "trstring.h"
#include "types.h"

// A buffer is an expanding wrapper
// around TrStr that allows auto-resizing
typedef struct Buffer {
  TrStr str;

  // where is the buffer currently at?
  usize space;
  Errors err;
} Buffer;

Buffer buffer_init();

// Resize buffer and copy from old to new location
void buffer_resize(Buffer *buffer, usize len);

// get next len for buffer
usize buffer_next_len(Buffer *buffer);

// Returns true if the next wite would overflow the buffer
bool buffer_full(Buffer *buffer);

void buffer_write(Buffer *buffer, char c);

void buffer_free(Buffer *buffer);

#ifdef TEST

void test_buffer_resize(void **state);

#endif
#endif
