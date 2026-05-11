# Chapter 30 Question 2 -- Summary of Changes

### object.h
```c
#define IS_STRING(value) \
    (isObjType(value, OBJ_STRING) || isObjType(value, OBJ_SHORT_STRING))
// ...
#define AS_CSTRING(value)      (stringChars(AS_STRING(value)))
// ...
typedef enum {
  OBJ_FUNCTION,
  OBJ_CLOSURE,
  OBJ_NATIVE,
  OBJ_STRING,
  OBJ_SHORT_STRING,
  OBJ_UPVALUE,
  OBJ_CLASS,
  OBJ_INSTANCE,
  OBJ_BOUND_METHOD,
} ObjType;
// ...
#define SHORT_STRING_MAX 7

typedef struct {
  Obj obj;
  int length;
  char chars[SHORT_STRING_MAX + 1];
  uint32_t hash;
} ObjShortString;
// ...
static inline int stringLength(ObjString* string) {
  if (string->obj.type == OBJ_SHORT_STRING) {
    return ((ObjShortString*)string)->length;
  }
  return string->length;
}

static inline const char* stringChars(ObjString* string) {
  if (string->obj.type == OBJ_SHORT_STRING) {
    return ((ObjShortString*)string)->chars;
  }
  return string->chars;
}

static inline uint32_t stringHash(ObjString* string) {
  if (string->obj.type == OBJ_SHORT_STRING) {
    return ((ObjShortString*)string)->hash;
  }
  return string->hash;
}
```

### object.c
```c
static ObjString* allocateShortString(const char* chars, int length,
                                      uint32_t hash) {
  ObjShortString* string = ALLOCATE_OBJ(ObjShortString, OBJ_SHORT_STRING);
  string->length = length;
  memcpy(string->chars, chars, (size_t)length);
  string->chars[length] = '\0';
  string->hash = hash;
  push(OBJ_VAL(string));
  tableSet(&vm.strings, (ObjString*)string, NIL_VAL);
  pop();
  return (ObjString*)string;
}
// ...
  if (length <= SHORT_STRING_MAX) {
    ObjString* string = allocateShortString(chars, length, hash);
    FREE_ARRAY(char, chars, length + 1);
    return string;
  }
// ...
  if (length <= SHORT_STRING_MAX) {
    return allocateShortString(chars, length, hash);
  }
// ...
  printf("<fn %s>", stringChars(function->name));
// ...
    case OBJ_STRING:
    case OBJ_SHORT_STRING:
      printf("%s", AS_CSTRING(value));
      break;
// ...
    case OBJ_CLASS:
      printf("%s", stringChars(AS_CLASS(value)->name));
      break;
    case OBJ_INSTANCE:
      printf("%s instance", stringChars(AS_INSTANCE(value)->klass->name));
      break;
```

### table.c
```c
  uint32_t index = stringHash(key) % capacity;
// ...
    } else if (stringLength(entry->key) == length &&
        stringHash(entry->key) == hash &&
        memcmp(stringChars(entry->key), chars, length) == 0) {
// ...
    if (entry->key != NULL && !((Obj*)entry->key)->isMarked) {
      tableDelete(table, entry->key);
    }
```

### memory.c
```c
    case OBJ_STRING:
    case OBJ_SHORT_STRING:
      break;
// ...
    case OBJ_SHORT_STRING:
      FREE(ObjShortString, object);
      break;
```

### vm.c
```c
      fprintf(stderr, "%s()\n", stringChars(function->name));
// ...
    runtimeError("Undefined property '%s'.", stringChars(name));
// ...
    runtimeError("Undefined property '%s'.", stringChars(name));
// ...
  int aLength = stringLength(a);
  int bLength = stringLength(b);
  int length = aLength + bLength;
  char* chars = ALLOCATE(char, length + 1);
  memcpy(chars, stringChars(a), (size_t)aLength);
  memcpy(chars + aLength, stringChars(b), (size_t)bLength);
// ...
          runtimeError("Undefined variable '%s'.", stringChars(name));
// ...
          runtimeError("Undefined variable '%s'.", stringChars(name));
```

### compiler.c
```c
    disassembleChunk(currentChunk(), function->name != NULL ? stringChars(function->name) : "<script>");
```

### Performance Comparison
```text
Short strings, length <= 7:
previous representation: one ObjString allocation plus one heap char buffer allocation
new representation: one ObjShortString allocation with chars stored inline

Long strings, length > 7:
unchanged ObjString allocation plus heap char buffer allocation

Expected effect:
new interned short strings remove one persistent heap allocation and one extra free per string
```
