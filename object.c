#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  return object;
}

static ObjString* allocateString(const char* chars, int length) {
  ObjString* string =
      (ObjString*)allocateObject(sizeof(ObjString) + (size_t)length + 1,
                                 OBJ_STRING);
  string->length = length;
  memcpy(string->chars, chars, (size_t)length);
  string->chars[length] = '\0';
  return string;
}

ObjString* copyString(const char* chars, int length) {
  return allocateString(chars, length);
}
