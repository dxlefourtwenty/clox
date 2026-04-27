# Chapter 25 Question 2 -- Summary of Changes

### compiler.c
```c
static void forStatement() {
  // ...
  beginScope();
  statement();
  endScope();
  emitLoop(loopStart);
  // ...
}
```
