# Chapter 19 Question 2 -- Summary of Changes

### object.h
```
// We would add a flag to each string
// object to indicate whether it owns its
// character array. Heap-copied strings
// would own their characters, while
// constant strings could point into existing
// memory without copying. When freeing,
// only owned character arrays would be
// released.
struct ObjString {
  Obj obj;
  int length;
  bool ownsChars;
  char* chars;
};
```
