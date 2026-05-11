# Chapter 29 Question 3 -- Summary of Changes

### chunk.h
```c
  OP_CALL,
  OP_INVOKE,
  OP_INNER,
  OP_SUPER_INVOKE,
```

### compiler.c
```c
static uint8_t argumentList() {
  uint8_t argCount = 0;
  if (!check(TOKEN_RIGHT_PAREN)) {
    do {
      expression();
      if (argCount == 255) {
        error("Can't have more than 255 arguments.");
      }
      argCount++;
    } while (match(TOKEN_COMMA));
  }

  consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
  return argCount;
}

// ...

static void inner_(bool canAssign) {
  (void)canAssign;
  if (currentClass == NULL ||
      (current->type != TYPE_METHOD && current->type != TYPE_INITIALIZER)) {
    error("Can't use 'inner' outside of a method.");
    return;
  }

  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'inner'.");
  emitBytes(OP_GET_LOCAL, 0);
  uint8_t argCount = argumentList();

  ObjString* name = current->function->name;
  uint8_t nameConstant = makeConstant(OBJ_VAL(name));
  emitBytes(OP_INNER, nameConstant);
  emitByte(argCount);
}

// ...

  [TOKEN_INNER] = {inner_, NULL, PREC_NONE},
```

### debug.c
```c
    case OP_INNER:
      return invokeInstruction("OP_INNER", chunk, offset);
```

### memory.c
```c
      markObject((Obj*)closure->owner);

// ...

      markTable(&((ObjClass*)object)->ownMethods);
      markObject((Obj*)((ObjClass*)object)->superclass);

// ...

      freeTable(&((ObjClass*)object)->ownMethods);
```

### object.c
```c
  closure->owner = NULL;

// ...

  initTable(&klass->ownMethods);
  klass->superclass = NULL;
```

### object.h
```c
typedef struct ObjClass ObjClass;

// ...

  ObjClass* owner;

// ...

struct ObjClass {
  Obj obj;
  ObjString* name;
  Table methods;
  Table ownMethods;
  ObjClass* superclass;
};
```

### scanner.c
```c
    case 'i':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'f': return checkKeyword(2, 0, "", TOKEN_IF);
          case 'n': return checkKeyword(2, 3, "ner", TOKEN_INNER);
        }
      }
      break;
```

### scanner.h
```c
  TOKEN_INNER, TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
```

### vm.c
```c
static void defineMethod(ObjString* name) {
  Value method = peek(0);
  ObjClass* klass = AS_CLASS(peek(1));
  AS_CLOSURE(method)->owner = klass;
  tableSet(&klass->ownMethods, name, method);
  if (!tableGet(&klass->methods, name, &method)) {
    tableSet(&klass->methods, name, peek(0));
  }
  pop();
}

static bool inner(ObjString* name, int argCount) {
  Value receiver = peek(argCount);
  if (!IS_INSTANCE(receiver)) {
    runtimeError("Only instances have inner methods.");
    return false;
  }

  ObjClass* owner = vm.frames[vm.frameCount - 1].closure->owner;
  if (owner == NULL) {
    runtimeError("Can't use 'inner' outside of a method.");
    return false;
  }

  int pathCount = 0;
  int pathCapacity = 0;
  ObjClass** path = NULL;
  for (ObjClass* klass = AS_INSTANCE(receiver)->klass;
       klass != NULL && klass != owner;
       klass = klass->superclass) {
    if (pathCapacity < pathCount + 1) {
      int oldCapacity = pathCapacity;
      pathCapacity = GROW_CAPACITY(pathCapacity);
      path = GROW_ARRAY(ObjClass*, path, oldCapacity, pathCapacity);
    }
    path[pathCount++] = klass;
  }

  Value method;
  bool found = false;
  for (int i = pathCount - 1; i >= 0; i--) {
    if (tableGet(&path[i]->ownMethods, name, &method)) {
      found = true;
      break;
    }
  }

  FREE_ARRAY(ObjClass*, path, pathCapacity);

  if (!found) {
    vm.stackTop -= argCount;
    vm.stackTop[-1] = NIL_VAL;
    return true;
  }

  return call(AS_CLOSURE(method), argCount);
}

// ...

      case OP_INNER: {
        ObjString* method = READ_STRING();
        int argCount = READ_BYTE();
        if (!inner(method, argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        frame = &vm.frames[vm.frameCount - 1];
        break;
      }

// ...

        subclass->superclass = AS_CLASS(superclass);
```
