#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hash_table.h"

int get_hash(const char* key, int m) {
    int combined_word = *key++;
    while(*key != '\0') {
        combined_word = (combined_word<<5)-combined_word + *key++;
    } 
    combined_word %= w;
    return (int)(m*fmod(A*abs(combined_word), 1));
}

void insert(Item** hashTable, Item item, int m) {
    int index = get_hash(item.key, m);
    while(hashTable[index] != NULL) {
        index = (index + 1) % m;
    }
    Item* i = malloc(sizeof(Item));
    i->key = item.key;
    i->flag = item.flag;
    i->addres = item.addres;
    hashTable[index] = i;
}

Item* search(Item** hashTable, const char* key, int m) {
    int index = get_hash(key, m);
    int i = 0;
    while(hashTable[index] != NULL && i < m) {
        if (strcmp(hashTable[index]->key, key) == 0) {
            return hashTable[index];
        }
        index = (index + 1) % m;
        i++;
    }
    return NULL;
}