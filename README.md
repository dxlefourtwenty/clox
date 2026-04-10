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
```
