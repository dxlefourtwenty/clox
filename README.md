# Chapter 20 Question 1 -- Summary of Changes

### Table.h
```
// changed from ObjString* to Value
typedef struct {
  Value key;
  Value value;
} Entry;

bool tableDelete(Table* table, Value key);
```

### Table.c
```
static Entry* findEntry(Entry* entries, int capacity,
                        Value key) {
  // changed key->hash to hashValue(key) so it works
  // for any valye type
  uint32_t index = hashValue(key) % capacity;
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (valuesEqual(entry->key, key)) {
      // changed from entry->key == key to valuesEqual()
      // so we can compare values instead of pointers
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

// ..

static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    // changed NULL to NIL_VAL since this creates an empty list
    entries[i].key = NIL_VAL;
    entries[i].value = NIL_VAL;
  }
// ..
// ..

bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  // use NIL_VAL key with an actual value to make a 
  // pseudo null value (tombstone)
  entry->key = NIL_VAL;
  entry->value = BOOL_VAL(true);
  return true;
}

static uint32_t hashValue(Value value) {
  switch (value.type) {
    case VAL_BOOL:
      return AS_BOOL(value) ? 3u : 1u;
    case VAL_NIL:
      return 2u;
    case VAL_NUMBER: {
      double number = AS_NUMBER(value);
      if (number == 0.0) number = 0.0;

      uint64_t bits;
      memcpy(&bits, &number, sizeof(double));
      return (uint32_t)(bits ^ (bits >> 32));
    }
    case VAL_OBJ:
      if (IS_STRING(value)) return AS_STRING(value)->hash;
      return (uint32_t)(uintptr_t)AS_OBJ(value);
  }

  return 0;
}
```
