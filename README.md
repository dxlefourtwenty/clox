# Chapter 19 Question 1 -- Summary of Changes

### object.h
```
typedef struct ObjString{
  Obj obj;
  int length;
  char chars[];
} ObjString;
```

### object.c
```
static ObjString* allocateString(const char* chars, int length) {
  ObjString* string =
      (ObjString*)allocateObject(sizeof(ObjString) + (size_t)length + 1,
                                 OBJ_STRING);
```
