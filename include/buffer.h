#ifndef BUFFER_H_
#define BUFFER_H_

#include "error.h"
#include "trstring.h"
#include "types.h"

// A buffer is an expanding wrapper
// around TrStr that allows auto-resizing
typedef struct StrBuffer {
  TrStr str;

  // where is the buffer currently at?
  usize cap;
  Errors err;
} StrBuffer;

StrBuffer strbuffer_init();

// Resize buffer and copy from old to new location
void strbuffer_resize(StrBuffer *buffer, usize len);

// get next len for buffer
usize strbuffer_next_len(StrBuffer *buffer);

// Returns true if the next wite would overflow the buffer
bool strbuffer_full(StrBuffer *buffer);

void strbuffer_write(StrBuffer *buffer, char c);

void strbuffer_append_raw(StrBuffer *buffer, char *s);

void strbuffer_free(StrBuffer *buffer);

#ifdef TEST

void test_strbuffer_resize(void **state);

#endif
#endif
