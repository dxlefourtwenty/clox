# Chapter 22 Question 4 -- Summary of Changes

### compiler.c (local storage + larger slot support)
```c
#define LOCALS_MAX 65536

typedef struct {
  Token name;
  int depth;
  bool isCaptured;
} Local;

typedef struct {
  Local locals[LOCALS_MAX];
  int localCount;
  int scopeDepth;
} Compiler;

static void emitShort(uint16_t value) {
  emitByte((uint8_t)(value & 0xff));
  emitByte((uint8_t)((value >> 8) & 0xff));
}

static void emitLocalInstruction(uint8_t shortOp, uint8_t longOp, int slot) {
  if (slot <= UINT8_MAX) {
    emitBytes(shortOp, (uint8_t)slot);
  } else if (slot <= UINT16_MAX) {
    emitByte(longOp);
    emitShort((uint16_t)slot);
  } else {
    error("Too many local variables in function.");
  }
}
```

### chunk.h (new opcodes)
```c
OP_GET_LOCAL,
OP_SET_LOCAL,
OP_GET_LOCAL_LONG,
OP_SET_LOCAL_LONG,
```

### vm.c (reads 2-byte local slot index)
```c
static uint16_t readSlotLong(void) {
  uint16_t low = *vm.ip++;
  uint16_t high = *vm.ip++;
  return (uint16_t)(low | (high << 8));
}

case OP_GET_LOCAL_LONG: {
  uint16_t slot = readSlotLong();
  push(vm.stack[slot]);
  break;
}
case OP_SET_LOCAL_LONG: {
  uint16_t slot = readSlotLong();
  vm.stack[slot] = peek(0);
  break;
}
```

### debug.c (disassembler support)
```c
static int shortInstruction(const char* name, Chunk* chunk, int offset) {
  uint16_t slot = (uint16_t)(chunk->code[offset + 1] |
                             (chunk->code[offset + 2] << 8));
  printf("%-16s %4d\n", name, slot);
  return offset + 3;
}

case OP_GET_LOCAL_LONG:
  return shortInstruction("OP_GET_LOCAL_LONG", chunk, offset);
case OP_SET_LOCAL_LONG:
  return shortInstruction("OP_SET_LOCAL_LONG", chunk, offset);
```

