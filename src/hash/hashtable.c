#include "hashtable.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#define LOAD_FACTOR 0.75
#define INITIAL_CAPACITY 8
#define GROWTH_FACTOR 2
#define PROBE_INDEX(index,capacity) (((index)+1)%capacity)
#define HASH_INDEX(hash,capacity) ((hash)%capacity)

uint64_t hash(char* key){
    uint64_t h = 0x100;
    size_t keyLength = strlen(key);
    for (size_t i = 0; i < keyLength; i++) {
        h ^= key[i] & 255;
        h *= 1111111111111111111;
    }
    return h;
}
void initTable(Table* table){
    table->count=0;
    table->capacity=INITIAL_CAPACITY;
    table->buckets=(Entry*)malloc(sizeof(Entry)*table->capacity);
    if(!table->buckets){
        fprintf(stderr,"Failure to allocate memory for buckets");
        return;
    }
    // Initialize all buckets
    for (size_t i = 0; i < table->capacity; i++) {
        table->buckets[i].key = NULL;
        table->buckets[i].value = NULL;
        table->buckets[i].state = EMPTY;
    }
}


KeyObject* allocateKeyObject(char* key){
    KeyObject* object = (KeyObject*)malloc(sizeof(KeyObject));
    if (!object) {
        fprintf(stderr, "Failed to allocate memory for KeyObject\n");
        return NULL;
    }
    size_t keyLength = strlen(key)+1;
    object->key = (char*)malloc(keyLength);
    if (!object->key) {
        fprintf(stderr, "Failed to allocate memory for key string\n");
        free(object);  // Clean up the previously allocated memory
        return NULL;
    }
    strcpy(object->key,key);
    object->hash = hash(key);
    return object;
}
void resizeTable(Table* table){
    Entry* oldBuckets = table->buckets;
    size_t oldCapacity = table->capacity;
    size_t newCapacity = table->capacity * GROWTH_FACTOR;
    Entry* newBuckets = (Entry*)malloc(sizeof(Entry)*newCapacity);
    if(!newBuckets){
        fprintf(stderr,"Failure to reallocate memory for buckets while resizing table");
        return;
    }
    for(size_t i=0;i<newCapacity;i++){
        newBuckets[i].key=NULL;
        newBuckets[i].value=NULL;
        newBuckets[i].state=EMPTY;
    }
    table->buckets=newBuckets;
    table->capacity=newCapacity;
    size_t oldCount = table->count;
    size_t oldTombstoneCount = table->tombstoneCount;
    table->count=0;
    table->tombstoneCount=0;
    for(size_t i=0;i<oldCapacity;i++){
        if(oldBuckets[i].state == OCCUPIED){
            size_t index = HASH_INDEX(oldBuckets[i].key->hash,newCapacity);
            while(newBuckets[index].state == OCCUPIED){
                index = PROBE_INDEX(index,newCapacity);
            }
            newBuckets[index]=oldBuckets[i];
            table->count++;
        }
    }
    if(oldCount!=table->count){
        printf("Actual Entries - %lu\nNew Table count - %lu",((oldCount)/sizeof(int))-((oldTombstoneCount)/sizeof(int)),(table->count)/sizeof(int));
    }
    free(oldBuckets);
}

void makeEntry(Table* table, char* key, void* val){
    KeyObject* object = allocateKeyObject(key);
    Entry entry;
    entry.key = object;
    entry.value = val;
    insertEntry(table,entry);
}

void insertEntry(Table* table, Entry entry){
    if(table->count+table->tombstoneCount>=table->capacity*LOAD_FACTOR){
        resizeTable(table);
    }
    size_t index = HASH_INDEX(entry.key->hash,table->capacity);
    size_t startIndex = index;
    size_t tombstoneIndex = SIZE_MAX;
    do{
        // ecncounter an empty entry so insert there
        if(table->buckets[index].state==EMPTY){
            if(tombstoneIndex!=SIZE_MAX){
                index=tombstoneIndex;
                table->tombstoneCount--;
            }
            break;
        }
        // encounter the first tombstone and keep track
        if(table->buckets[index].state==TOMBSTONE && tombstoneIndex==SIZE_MAX){
            tombstoneIndex=index;
        }
        // encounter the same key so update the value
        if(table->buckets[index].state==OCCUPIED && table->buckets[index].key->hash==entry.key->hash && strcmp(table->buckets[index].key->key,entry.key->key)==0){
            free(table->buckets[index].key->key);
            free(table->buckets[index].key);
            table->buckets[index]=entry;
            table->buckets[index].state=OCCUPIED;
            return;
        }
        index=PROBE_INDEX(index,table->capacity);
    } while(index!=startIndex);
    
    table->buckets[index]=entry;
    table->buckets[index].state=OCCUPIED;
    table->count++;
}

bool deleteEntry(Table* table, char* key){
    if(table->count==0){
        return false;
    }
    uint64_t hashValue = hash(key);
    size_t index = HASH_INDEX(hashValue,table->capacity);
    size_t startIndex = index;
    do{
        if(table->buckets[index].state==EMPTY){
            return false;
        }
        if(table->buckets[index].state==OCCUPIED && table->buckets[index].key->hash==hashValue && strcmp(table->buckets[index].key->key,key)==0){
            free(table->buckets[index].key->key);
            free(table->buckets[index].key);
            table->buckets[index].key = NULL;
            table->buckets[index].value = NULL;
            table->buckets[index].state=TOMBSTONE;
            table->tombstoneCount++;
            table->count--;
            return true;
        }
        index=PROBE_INDEX(index,table->capacity);
    }while(index!=startIndex);
    return false;
}
void *getEntry(Table* table, char* key){
    if(table->count==0){
        return NULL;
    }
    uint64_t hashValue = hash(key);
    size_t index = HASH_INDEX(hashValue,table->capacity);
    size_t startIndex = index;
    do{
        if(table->buckets[index].state==EMPTY){
            return NULL;
        }
        if(table->buckets[index].state==OCCUPIED && table->buckets[index].key->hash==hashValue && strcmp(table->buckets[index].key->key,key)==0){
            return table->buckets[index].value;
        }
        index=PROBE_INDEX(index,table->capacity);
    }while(index!=startIndex);
    return NULL;
}
void freeTable(Table* table){
    for(size_t i=0;i<table->capacity;i++){
        if(table->buckets[i].state==OCCUPIED){
            free(table->buckets[i].key->key);
            free(table->buckets[i].key);
        }
    }
    free(table->buckets);
    table->count=0;
    table->capacity=0;
    table->buckets=NULL;
}