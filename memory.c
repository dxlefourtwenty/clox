#include <stdlib.h>

#include "memory.h"

// the two size arguments control which operation to perform 
//
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);

  // if realloc somehow fails
  // prevent the return of a NULL pointer
  if (result == NULL) exit(1);
  return result;
}
