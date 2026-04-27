# Chapter 24 Question 3 -- Summary of Changes

### object.h
```c
typedef bool (*NativeFn)(int argCount, Value* args, Value* result);
```

### vm.c
```c
#include <math.h>

static void runtimeError(const char* format, ...);

static bool clockNative(int argCount, Value* args, Value* result) {
  (void)argCount;
  (void)args;
  *result = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
  return true;
}

static bool sqrtNative(int argCount, Value* args, Value* result) {
  if (argCount != 1) {
    runtimeError("sqrt() expects 1 argument.");
    return false;
  }

  if (!IS_NUMBER(args[0])) {
    runtimeError("sqrt() argument must be a number.");
    return false;
  }

  double x = AS_NUMBER(args[0]);
  if (x < 0) {
    runtimeError("sqrt() argument must be non-negative.");
    return false;
  }

  *result = NUMBER_VAL(sqrt(x));
  return true;
}

// ...

  defineNative("clock", clockNative);
  defineNative("sqrt", sqrtNative);

// ...

      case OBJ_NATIVE: {
        NativeFn native = AS_NATIVE(callee);
        Value result;
        if (!native(argCount, vm.stackTop - argCount, &result)) {
          return false;
        }
        vm.stackTop -= argCount + 1;
        push(result);
        return true;
      }
```

