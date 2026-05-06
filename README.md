# Chapter 27 Question 3 -- Summary of Changes

vm.c
```c
static Value deleteFieldNative(int argCount, Value* args) {
  if (argCount != 2) return NIL_VAL;
  if (!IS_INSTANCE(args[0])) return NIL_VAL;
  if (!IS_STRING(args[1])) return NIL_VAL;

  ObjInstance* instance = AS_INSTANCE(args[0]);
  tableDelete(&instance->fields, AS_STRING(args[1]));
  return NIL_VAL;
}

// ...

  defineNative("deleteField", deleteFieldNative);
```
