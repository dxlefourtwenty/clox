# Chapter 21 Question 1 -- Summary of Changes

### compiler.c
```
static uint8_t identifierConstant(Token* name) {
  // check if this variable name is already in the constants table
  for (int i = 0; i < currentChunk()->constants.count; i++) {
    Value existing = currentChunk()->constants.values[i];
    if (!IS_STRING(existing)) continue; // skip non-string constants
    ObjString* existingStr = AS_STRING(existing);

    // check if the lengths and characters match
    if (existingStr->length == name->length &&
        memcmp(existingStr->chars, name->start, (size_t)name->length) == 0) {
      return (uint8_t)i; // reuse the existing slot
    }
  }

  return makeConstant(OBJ_VAL(copyString(name->start, name->length))); // use new slot
}
```
