#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "common.h"

#define HASH_TABLE_MAX_SIZE 64

STRUCT(HashTable) {
  usize size;
  i32 data[HASH_TABLE_MAX_SIZE];
  bool used[HASH_TABLE_MAX_SIZE];
};

void hash_table_init(HashTable *self);
bool hash_table_add(HashTable *self, const char *key, i32 value);
bool hash_table_has(HashTable *self, const char *key);
i32 hash_table_get(HashTable *self, const char *key);
void hash_table_remove(HashTable *self, const char *key);

#endif
