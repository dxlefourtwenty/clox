# Chapter 23 Question 1 -- Summary of Changes

### vm.c
```c
static Value getFieldNative(int argCount, Value* args) {
  if (argCount != 2) return BOOL_VAL(false);
  if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
  if (!IS_STRING(args[1])) return BOOL_VAL(false);

  ObjInstance* instance = AS_INSTANCE(args[0]);
  ObjString* fieldName = AS_STRING(args[1]);
  Value value;
  if (!tableGet(&instance->fields, fieldName, &value)) return NIL_VAL;

  return value;
}

static Value setFieldNative(int argCount, Value* args) {
  if (argCount != 3) return BOOL_VAL(false);
  if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
  if (!IS_STRING(args[1])) return BOOL_VAL(false);

  ObjInstance* instance = AS_INSTANCE(args[0]);
  ObjString* fieldName = AS_STRING(args[1]);
  tableSet(&instance->fields, fieldName, args[2]);

  return args[2];
}

// ...

  defineNative("getField", getFieldNative);
  defineNative("setField", setFieldNative);
```
