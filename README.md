# Chapter 24 Question 4 -- Summary of Changes

### vm.c
```c
static void runtimeError(const char* format, ...);

static Value typeNative(int argCount, Value* args) {
  if (argCount != 1) {
    runtimeError("type() expects 1 argument.");
    return NIL_VAL;
  }

  Value value = args[0];
  if (IS_NUMBER(value)) {
    return OBJ_VAL(copyString("number", 6));
  }

  if (IS_BOOL(value)) {
    return OBJ_VAL(copyString("bool", 4));
  }

  if (IS_NIL(value)) {
    return OBJ_VAL(copyString("nil", 3));
  }

  if (IS_STRING(value)) {
    return OBJ_VAL(copyString("string", 6));
  }

  if (IS_FUNCTION(value)) {
    return OBJ_VAL(copyString("function", 8));
  }

  if (IS_NATIVE(value)) {
    return OBJ_VAL(copyString("native", 6));
  }

  return OBJ_VAL(copyString("unknown", 7));
}

// ...

defineNative("type", typeNative);
```
