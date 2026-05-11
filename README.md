# Chapter 28 Question 1 -- Summary of Changes

### object.h
```c
typedef struct {
  Obj obj;
  ObjString* name;
  Value initializer;
  Table methods;
} ObjClass;
```

### object.c
```c
ObjClass* newClass(ObjString* name) {
  ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
  klass->name = name;
  klass->initializer = NIL_VAL;
  initTable(&klass->methods);
  return klass;
}
```

### vm.c
```c
case OBJ_CLASS: {
  ObjClass* klass = AS_CLASS(callee);
  vm.stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));
  if (!IS_NIL(klass->initializer)) {
    return call(AS_CLOSURE(klass->initializer), argCount);
  }

  if (argCount != 0) {
    runtimeError("Expected 0 arguments but got %d.", argCount);
    return false;
  }

  return true;
}

// ...

static void defineMethod(ObjString* name) {
  Value method = peek(0);
  ObjClass* klass = AS_CLASS(peek(1));
  tableSet(&klass->methods, name, method);
  if (name == vm.initString) {
    klass->initializer = method;
  }
  pop();
}
```

### memory.c
```c
case OBJ_CLASS:
  markObject((Obj*)((ObjClass*)object)->name);
  markValue(((ObjClass*)object)->initializer);
  markTable(&((ObjClass*)object)->methods);
  break;
```
