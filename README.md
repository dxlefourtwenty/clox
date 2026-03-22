# Chapter 15 Question 3 -- Summary of Changes

### vm.h
```
typedef struct {
  Chunk* chunk;
  uint8_t* ip;

  Value* stack;
  Value* stackTop;
  int stackCapacity;
} VM;
```

### vm.c
```
#include <stdlib.h>

void initVM() {
  vm.stackCapacity = 256;
  vm.stack = malloc(sizeof(Value) * vm.stackCapacity);
  resetStack();
}

void freeVM() {
  free(vm.stack);
}

void push(Value value) {
  if (vm.stackTop - vm.stack >= vm.stackCapacity) {
    vm.stackCapacity *= 2;
    vm.stack = realloc(vm.stack, sizeof(Value) * vm.stackCapacity);
    vm.stackTop = vm.stack + vm.stackCapacity / 2;
  }

  *vm.stackTop = value;
  vm.stackTop++;
}
```

### Costs and Benefits of Changes
```
Benefits
  - Prevents stack overflow crashes
  - Allows programs with large expressions
  - More flexible and scalable

Costs
  - Slight performance overhead
  - Memory allocation during runtime
  - More complex implementation
```
