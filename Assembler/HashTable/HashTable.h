#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "List.h"

const size_t TABLE_LEN = 100007;
const size_t MAX_LIST_LEN = 2;

typedef struct HashTable_
{
    List* lists[TABLE_LEN];
    int (*cmp) (const void*, const void*);
    unsigned long long int (*hash) (const void*);
} HashTable;

HashTable*  HashTableCtr (size_t itemSize, unsigned long long int (*hash) (const void*), int (*cmp) (const void*, const void*));
void        HashTableDtr (HashTable* table);

void*       HashTableFind (HashTable* table, const void* key, bool create = false);

bool        HashTableDump (HashTable* table, void (*datadump) (FILE* file, const void* item) = nullptr);
bool        HashTableDump (HashTable* table, FILE* file, void (*datadump) (FILE* file, const void* item) = nullptr);

bool        HashTableOk   (HashTable* table);

#endif
