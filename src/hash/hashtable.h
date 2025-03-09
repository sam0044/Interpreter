#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum{
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} EntryState;

typedef struct{
    char* key;
    uint64_t hash;
} KeyObject;

typedef struct{
    KeyObject* key;
    void* value;
    EntryState state;
} Entry;

typedef struct{
    Entry* buckets;
    size_t count;
    size_t capacity;
    size_t tombstoneCount;
} Table;

uint64_t hash(char* key);
KeyObject* allocateKeyObject(char* key);
void makeEntry(Table* table, char* key, void* val);
void initTable(Table* table);
void resizeTable(Table* table);
void insertEntry(Table* table, Entry entry);
bool deleteEntry(Table* table, char* key);
void *getEntry(Table* table, char* key);
void freeTable(Table* table);





#endif