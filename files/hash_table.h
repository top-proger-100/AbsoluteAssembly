#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

# define w 101
# define A 0.618033988749894848204586834365638117720309

typedef struct {
    char* key;
    bool flag;
    int addres;
} Item; 

int get_hash(const char* key, int m);
void insert(Item** hashTable, Item item, int m);
Item* search(Item** hashTable, const char* key, int m);

#endif