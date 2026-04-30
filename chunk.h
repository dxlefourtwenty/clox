// chunk refers to sequences of bytecode

#ifndef clox_chunk_h

#define clox_chunk_h

#include "common.h"
#include "value.h" 

// in our bytecode format, each instruction has a one-byte
// operation code (opcode) <-- this number controls what kind
// of instruction we're dealing with (e.g. add, subtract, look up variables, etc.)
typedef enum {
  OP_CONSTANT,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
  OP_SET_GLOBAL,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_CONSTANT_LONG,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_LOOP,
  OP_CALL,
  OP_INVOKE,
  OP_CLOSURE,
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  OP_CLASS,
  OP_METHOD,
  OP_CLOSE_UPVALUE,
  OP_RETURN,
} OpCode;

// at the moment, this is simply a wrapper around an array of bytes
// this is dynamic because we don't know how big the array needs to be before we start compiling a chunk
typedef struct {
  int offset;
  int line;
} LineStart;

typedef struct {
  int count;
  int capacity;
  uint8_t* code;
  ValueArray constants;
  int lineCount;
  int lineCapacity;
  LineStart* lines;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line);
int getLine(Chunk* chunk, int instruction);

#endif
