# Chapter 23 Question 1 -- Summary of Changes

### vm.c
```c
static Value hasFieldNative(int argCount, Value* args) {
  if (argCount != 2) return BOOL_VAL(false);
  if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
  if (!IS_STRING(args[1])) return BOOL_VAL(false);

  ObjInstance* instance = AS_INSTANCE(args[0]);
  Value dummy;
  return BOOL_VAL(tableGet(&instance->fields, AS_STRING(args[1]), &dummy));
}

// ...

defineNative("hasField", hasFieldNative);
```
