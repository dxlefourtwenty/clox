# Chapter 23 Question 1 -- Summary of Changes

### object.h
```c
struct Obj {
  ObjType type;
  bool isMarked;
  int refCount;
  struct Obj* next;
};
```

### object.c
```c
static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->isMarked = false;
  object->refCount = 0;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

// ...

ObjClosure* newClosure(ObjFunction* function) {
  ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
  for (int i = 0; i < function->upvalueCount; i++) {
    upvalues[i] = NULL;
  }

  ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;
  incRef((Obj*)function);
  closure->upvalues = upvalues;
  closure->upvalueCount = function->upvalueCount;
  return closure;
}
```

### memory.h
```c
void* reallocate(void* pointer, size_t oldSize, size_t newSize);
void beginFreeObjects();
void freeObjects();
void incRef(Obj* object);
void decref(Obj* object);
void incrementValue(Value value);
void decrementValue(Value value);
void decrementArray(ValueArray* array);
void markObject(Obj* object);
void markValue(Value value);
void collectGarbage();
```

### memory.c
```c
static void freeObject(Obj* object);
static bool isShuttingDown = false;

// ...

static void sweep() {
  Obj* previous = NULL;
  Obj* object = vm.objects;
  while (object != NULL) {
    if (object->refCount > 0) {
      object->isMarked = false;
      previous = object;
      object = object->next;
    } else {
      Obj* unreached = object;
      object = object->next;
      if (previous != NULL) {
        previous->next = object;
      } else {
        vm.objects = object;
      }

      freeObject(unreached);
    }
  }
}

void incRef(Obj* object) {
  if (object == NULL) return;
  object->refCount++;
}

void decref(Obj* object) {
  if (object == NULL) return;
  if (isShuttingDown) return;
  object->refCount--;
}

void incrementValue(Value value) {
  if (IS_OBJ(value)) incRef(AS_OBJ(value));
}

void decrementValue(Value value) {
  if (IS_OBJ(value)) decref(AS_OBJ(value));
}

void decrementArray(ValueArray* array) {
  for (int i = 0; i < array->count; i++) {
    decrementValue(array->values[i]);
  }
}

// ...

static void freeObject(Obj* object) {
  switch (object->type) {
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      decref((Obj*)closure->function);
      for (int i = 0; i < closure->upvalueCount; i++) {
        decref((Obj*)closure->upvalues[i]);
      }
      FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
      FREE(ObjClosure, object);
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      decref((Obj*)function->name);
      freeChunk(&function->chunk);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_NATIVE:
      FREE(ObjNative, object);
      break;
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, object);
      break;
    }
    case OBJ_UPVALUE:
      decrementValue(((ObjUpvalue*)object)->closed);
      FREE(ObjUpvalue, object);
      break;
  }
}

void freeObjects() {
  isShuttingDown = true;
  Obj* object = vm.objects;
  while (object != NULL) {
    Obj* next = object->next;
    freeObject(object);
    object = next;
  }
}

void beginFreeObjects() {
  isShuttingDown = true;
}
```

### value.c
```c
void writeValueArray(ValueArray* array, Value value) {
  if (array->capacity < array->count + 1) {
    int oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values,
                               oldCapacity, array->capacity);
  }

  array->values[array->count] = value;
  incrementValue(value);
  array->count++;
}

void freeValueArray(ValueArray* array) {
  decrementArray(array);
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}
```

### table.c
```c
void freeTable(Table* table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key != NULL) {
      decref((Obj*)entry->key);
      decrementValue(entry->value);
    }
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

// ...

bool tableSet(Table* table, ObjString* key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) table->count++;
  if (isNewKey) {
    incRef((Obj*)key);
  } else {
    decrementValue(entry->value);
  }

  entry->key = key;
  entry->value = value;
  incrementValue(value);
  return isNewKey;
}

bool tableDelete(Table* table, ObjString* key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  // Place a tombstone in the entry.
  decref((Obj*)entry->key);
  decrementValue(entry->value);
  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}
```

### compiler.c
```c
static ObjFunction* endCompiler() {
  emitReturn();
  ObjFunction* function = current->function;

#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassembleChunk(currentChunk(), function->name != NULL ? function->name->chars : "<script>");
  }
#endif

  current = current->enclosing;
  decref((Obj*)function);
  return function;
}

// ...

static void initCompiler(Compiler* compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = NULL;
  compiler->type = type;
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  compiler->function = newFunction();
  incRef((Obj*)compiler->function);
  current = compiler;

  if (type != TYPE_SCRIPT) {
    current->function->name = copyString(parser.previous.start, parser.previous.length);
    incRef((Obj*)current->function->name);
  }

  Local* local = &current->locals[current->localCount++];
  local->depth = 0;
  local->isCaptured = false;
  local->name.start = "";
  local->name.length = 0;
}
```

### vm.c
```c
static void resetStack() {
  while (vm.stackTop != NULL && vm.stackTop > vm.stack) {
    pop();
  }

  vm.stackTop = vm.stack;
  vm.frameCount = 0;
  vm.openUpvalues = NULL;
}

// ...

void freeVM() {
  beginFreeObjects();
  freeTable(&vm.globals);
  freeTable(&vm.strings);
  freeObjects();
  free(vm.grayStack);
}

void push(Value value) {
  incrementValue(value);
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop() {
  vm.stackTop--;
  Value value = *vm.stackTop;
  decrementValue(value);
  return value;
}

static void setSlot(Value* slot, Value value) {
  incrementValue(value);
  decrementValue(*slot);
  *slot = value;
}

// ...

static void closeUpvalues(Value* last) {
  while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
    ObjUpvalue* upvalue = vm.openUpvalues;
    upvalue->closed = *upvalue->location;
    incrementValue(upvalue->closed);
    upvalue->location = &upvalue->closed;
    vm.openUpvalues = upvalue->next;
  }
}

// ...

static bool callValue(Value callee, int argCount) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
      case OBJ_CLOSURE:
        return call(AS_CLOSURE(callee), argCount);
      case OBJ_NATIVE: {
        NativeFn native = AS_NATIVE(callee);
        Value result = native(argCount, vm.stackTop - argCount);
        for (int i = 0; i < argCount + 1; i++) {
          pop();
        }
        push(result);
        return true;
      }
      default:
        break;
    }
  }

  runtimeError("Can only call functions and classes.");
  return false;
}

// ...

static void concatenate() {
  ObjString* b = AS_STRING(peek(0));
  ObjString* a = AS_STRING(peek(1));

  int length = a->length + b->length;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString* result = takeString(chars, length);
  pop();
  pop();
  push(OBJ_VAL(result));
}

// ...

case OP_SET_LOCAL: {
  uint8_t slot = READ_BYTE();
  setSlot(&frame->slots[slot], peek(0));
  break;
}

// ...

case OP_SET_UPVALUE: {
  uint8_t slot = READ_BYTE();
  setSlot(frame->closure->upvalues[slot]->location, peek(0));
  break;
}

// ...

case OP_CLOSURE: {
  ObjFunction* function = AS_FUNCTION(READ_CONSTANT());
  ObjClosure* closure = newClosure(function);
  push(OBJ_VAL(closure));
  for (int i = 0; i < closure->upvalueCount; i++) {
    uint8_t isLocal = READ_BYTE();
    uint8_t index = READ_BYTE();
    if (isLocal) {
      closure->upvalues[i] = captureUpvalue(frame->slots + index);
    } else {
      closure->upvalues[i] = frame->closure->upvalues[index];
    }
    incRef((Obj*)closure->upvalues[i]);
  }
  break;
}
```

### debug.c
```c
static int closureInstruction(const char* name, Chunk* chunk, int offset) {
  offset++;
  uint8_t constant = chunk->code[offset++];
  printf("%-16s %4d ", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("\n");

  ObjFunction* function = AS_FUNCTION(chunk->constants.values[constant]);
  for (int j = 0; j < function->upvalueCount && offset + 1 < chunk->count; j++) {
    int isLocal = chunk->code[offset++];
    int index = chunk->code[offset++];
    printf("%04d      |                     %s %d\n", offset - 2,
           isLocal ? "local" : "upvalue", index);
  }

  return offset;
}
```
