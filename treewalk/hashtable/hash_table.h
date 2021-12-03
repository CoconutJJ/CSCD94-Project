#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "hashtable_types.h"

struct hashtable *hashtable_new();
void hashtable_delete(struct hashtable *t, char *key);
void hashtable_set(struct hashtable *t, char *key, void *item, int item_sz);
void *hashtable_get(struct hashtable *t, char *key);
int hashtable_has(struct hashtable *t, char *key);
void hashtable_destroy(struct hashtable *table);
void initialize_hashtable(struct hashtable *t);
#endif