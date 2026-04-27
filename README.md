# Chapter 24 Question 2 -- Summary of Changes

### object.h
```c
typedef struct {
  Obj obj;
  NativeFn function;
  int arity;
} ObjNative;

ObjNative* newNative(NativeFn function, int arity);
```

### object.c
```c
ObjNative* newNative(NativeFn function, int arity) {
  ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  native->arity = arity;
  return native;
}
```

### vm.c
```c
static void defineNative(const char* name, NativeFn function, int arity) {
  push(OBJ_VAL(copyString(name, (int)strlen(name))));
  push(OBJ_VAL(newNative(function, arity)));
  tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
  pop();
  pop();
}
// ...
  defineNative("clock", clockNative, 0);
// ...
      case OBJ_NATIVE: {
        ObjNative* native = (ObjNative*)AS_OBJ(callee);
        if (argCount != native->arity) {
          runtimeError("Expected %d arguments but got %d.",
              native->arity, argCount);
          return false;
        }
        Value result = native->function(argCount, vm.stackTop - argCount);
        vm.stackTop -= argCount + 1;
        push(result);
        return true;
      }
```
