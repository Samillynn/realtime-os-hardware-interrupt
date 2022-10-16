#include "hash_table.h"

static usize hash_table_djb2(const char* str) {
  usize hash = 5381;
  char ch;

  while (ch = *str++) {
    hash = ((hash << 5) + hash) + (usize)ch;
  }

  return hash;
}

void hash_table_init(HashTable* self) {
  self->size = HASH_TABLE_MAX_SIZE;

  for (usize i = 0; i < self->size; i ++) {
    self->used[i] = false;
  }
}

bool hash_table_add(HashTable* self, const char* key, i32 value) {
  usize hash = hash_table_djb2(key) % self->size;

  if (!self->used[hash]) {
    self->used[hash] = true;
    self->data[hash] = value;

    return true;
  }

  return false;
}

bool hash_table_has(HashTable* self, const char* key) {
  return self->used[hash_table_djb2(key) % self->size];
}

i32 hash_table_get(HashTable* self, const char* key) {
  usize hash = hash_table_djb2(key) % self->size;

  if (hash < self->size && self->used[hash]) {
    return self->data[hash];
  }

  return 0;
}

void hash_table_remove(HashTable* self, const char* key) {
  usize hash = hash_table_djb2(key) % self->size;
  self->used[hash] = false;
}
