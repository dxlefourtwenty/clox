#ifndef clox_memory_h

#define clox_memory_h

#include "common.h"

// this macro calculates a new capacity based
// on a given current capacity
// this scales on the old size for performance reasons
// grow by a factor of 2
// (1.5 is common also)
#define GROW_CAPACITY(capacity) \
  ((capacity) < 8 ? 8 : (capacity) * 2)

// takes care of getting the size of the array's element
// type and casting the resulting void* back to a pointer of
// the right type
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
  (type*)reallocate(pointer, sizeof(type) * (oldCount), \
      sizeof(type) * (newCount))

// frees the memory by passing in zero for the new size
#define FREE_ARRAY(type, pointer, oldCount) \
  reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif
