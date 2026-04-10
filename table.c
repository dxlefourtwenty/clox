#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

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

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity,
                        Value key) {
  uint32_t index = hashValue(key) % (uint32_t)capacity;
  Entry* tombstone = NULL;

  for (;;) {
    Entry* entry = &entries[index];
    if (!entry->isOccupied) {
      if (!entry->isTombstone) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (valuesEqual(entry->key, key)) {
      // We found the key.
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

bool tableGet(Table* table, Value key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->isOccupied) return false;

  *value = entry->value;
  return true;
}

static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NIL_VAL;
    entries[i].value = NIL_VAL;
    entries[i].isOccupied = false;
    entries[i].isTombstone = false;
  }

  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (!entry->isOccupied) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    dest->isOccupied = true;
    dest->isTombstone = false;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

bool tableSet(Table* table, Value key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = !entry->isOccupied;
  if (isNewKey && !entry->isTombstone) table->count++;

  entry->isOccupied = true;
  entry->isTombstone = false;
  entry->key = key;
  entry->value = value;
  return isNewKey;
}

void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->isOccupied) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->isOccupied) return false;

  // Tombstone entry.
  entry->isOccupied = false;
  entry->isTombstone = true;
  entry->key = NIL_VAL;
  entry->value = NIL_VAL;
  return true;
}


ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;
  for (;;) {
    Entry* entry = &table->entries[index];
    if (!entry->isOccupied) {
      // Stop if we find an empty non-tombstone entry.
      if (!entry->isTombstone) return NULL;
    } else if (IS_STRING(entry->key)) {
      ObjString* key = AS_STRING(entry->key);
      if (key->length == length &&
          key->hash == hash &&
          memcmp(key->chars, chars, (size_t)length) == 0) {
      // We found it.
        return key;
      }
    }

    index = (index + 1) % table->capacity;
  }
}
