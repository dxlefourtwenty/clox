# Chapter 18 Question 1 -- Summary of Changes

### compiler.c
```
switch (operatorType) {
  // ...
  case TOKEN_PLUS:          emitByte(OP_ADD); break;
  // change this from OP_SUBTRACT TO OP_NEGATE, OP_ADD
  case TOKEN_MINUS:         emitBytes(OP_NEGATE, OP_ADD); break;
  case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
  case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
  default:
    return; // Unreachable.
}
```
