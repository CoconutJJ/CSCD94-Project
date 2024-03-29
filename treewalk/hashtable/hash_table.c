#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable_types.h"

/**
 * Linear probing hashtable.
 */

unsigned long hash(char *key) {
        /**
         * djb2 hash method
         */
        unsigned long hash = 5381;
        int c;

        while ((c = *key++)) hash = ((hash << 5) + hash) ^ c;

        return hash;
}

void initialize_containers(struct hashtable *t) {
        for (int i = 0; i < t->size; i++) {
                t->table[i].key = NULL;
                t->table[i].item = NULL;
                t->table[i].deleted = 0;
        }
}

void initialize_hashtable(struct hashtable *t) {
        t->table = malloc(sizeof(struct hashcontainer));
        t->table->key = NULL;
        t->table->item = NULL;
        t->filled = 0;
        t->size = 1;
}

/**
 * Create a new hashtable
 */
struct hashtable *hashtable_new() {
        struct hashtable *new = malloc(sizeof(struct hashtable));

        if (!new) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        new->table = malloc(sizeof(struct hashcontainer));

        if (!new->table) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        new->table->key = NULL;
        new->table->item = NULL;
        new->filled = 0;
        new->size = 1;

        return new;
}
/**
 * Destroy the hash table
 */
void hashtable_destroy(struct hashtable *table) {
        for (int i = 0; i < table->size; i++) {
                if (table->table[i].key != NULL) {
                        if (table->table[i].item) free(table->table[i].item);
                        free(table->table[i].key);
                }
        }
        free(table->table);
        free(table);
}

/**
 * Sets an entry in the hashtable
 */
void hashtable_find_and_set(struct hashtable *t, char *key, void *item) {
        assert(t->filled != t->size);

        int index = hash(key) % t->size, currIndex = index;

        struct hashcontainer *node;
        struct hashcontainer *free_slot = NULL;
        while (1) {
                node = &(t->table[currIndex]);

                /**
                 * Always set value in first empty slot
                 */
                if (node->key == NULL && free_slot == NULL) {
                        free_slot = node;

                        /**
                         * We've hit a never used before empty cell, key cannot
                         * possibly exist.
                         */
                        if (!free_slot->deleted) break;
                }

                /*
                 * If key already exists, destroy old key and item
                 */
                if (node->key && strcmp(node->key, key) == 0) {
                        free(node->key);
                        if (node->item) free(node->item);

                        node->item = NULL;
                        node->key = NULL;
                        node->deleted = 1;

                        /**
                         * If we already found an empty slot, use that one
                         * instead (since it occurs earlier), otherwise empty
                         * this slot and use this one.
                         */
                        if (!free_slot) free_slot = node;

                        break;
                }

                currIndex = (currIndex + 1) % t->size;

                if (currIndex == index) {
                        assert(free_slot != NULL);
                        break;
                }
        }

        free_slot->key = key;
        free_slot->item = item;
        free_slot->deleted = 0;

        t->filled++;
}

/**
 * Expand hashtable by a factor of 2
 */
void expand_hashtable(struct hashtable *t) {
        /**
         * Here we detach the original containers list and create a fresh one
         * of 2 times the size
         */
        struct hashcontainer *new_container =
            malloc(2 * t->size * sizeof(struct hashcontainer));

        if (!new_container) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        struct hashcontainer *old_container = t->table;

        t->table = new_container;
        t->size = 2 * t->size;
        t->filled = 0;

        initialize_containers(t);

        /**
         * We must rehash and insert into new container list.
         */
        for (int i = 0; i < t->size / 2; i++) {
                if (old_container[i].deleted || old_container[i].key == NULL)
                        continue;

                hashtable_find_and_set(t, old_container[i].key,
                                       old_container[i].item);
        }
        /**
         * Destroy the old one.
         */
        free(old_container);
}

/**
 * Shrink the hashtable by a factor of 2. If size is unit, then nothing is done.
 */
void shrink_hashtable(struct hashtable *t) {
        if (t->size == 1) return;

        struct hashcontainer *new_container =
            malloc(t->size / 2 * sizeof(struct hashcontainer));

        if (!new_container) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        struct hashcontainer *old_container = t->table;

        t->table = new_container;
        t->size = t->size / 2;
        t->filled = 0;

        initialize_containers(t);

        for (int i = 0; i < t->size * 2; i++) {
                if (old_container[i].deleted || old_container[i].key == NULL)
                        continue;

                hashtable_find_and_set(t, old_container[i].key,
                                       old_container[i].item);
        }
        free(old_container);
}
/**
 * Delete the entry from the hashtable
 */
void hashtable_delete(struct hashtable *t, char *key) {
        struct hashcontainer *node;

        int index = hash(key) % t->size, currIndex = index;

        /**
         * Find the entry that contains the key-value pair.
         */
        while (strcmp((node = &(t->table[currIndex]))->key, key) != 0) {
                currIndex = (currIndex + 1) % t->size;

                if (currIndex == index) return;
        }

        free(node->key);

        if (node->item) free(node->item);

        node->key = NULL;
        node->item = NULL;
        node->deleted = 1;

        t->filled--;

        /**
         * Only shrink if used space has dropped to less than one quarter of
         * allocated space
         */
        if (t->filled <= t->size / 4) shrink_hashtable(t);
}

/**
 * Return a pointer to the entry with provided key.
 */
void *hashtable_get(struct hashtable *t, char *key) {
        int index = hash(key) % t->size, currIndex = index;
        while (1) {
                /**
                 * If entry was deleted then just skip
                 */

                if (t->table[currIndex].deleted) {
                        currIndex = (currIndex + 1) % t->size;
                        if (currIndex == index) return NULL;
                        continue;
                }

                /**
                 * Cell was never used, key must not be in table.
                 */
                if (t->table[currIndex].key == NULL) {
                        return NULL;
                }

                if (strcmp(t->table[currIndex].key, key) == 0) {
                        return t->table[currIndex].item;
                }

                currIndex = (currIndex + 1) % t->size;

                /**
                 * We have looped through everything, entry clearly does not
                 * exist.
                 */
                if (currIndex == index) return NULL;
        }
}

/**
 * Sets an entry in the table.
 */
void hashtable_set(struct hashtable *t, char *key, void *item, int item_sz) {
        /**
         * Need to ensure we have enough space for another entry, expand
         * otherwise.
         */
        if (t->filled == t->size) expand_hashtable(t);

        /**
         * Copy the key and item data, external pointers may go out of scope
         * or be freed.
         */
        char *memkey = malloc(strlen(key) + 1);

        if (!memkey) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        strcpy(memkey, key);

        void *memitem = NULL;
        if (item) {
                memitem = malloc(item_sz);

                if (!memitem) {
                        perror("malloc");
                        exit(EXIT_FAILURE);
                }

                memcpy(memitem, item, item_sz);
        }
        hashtable_find_and_set(t, memkey, memitem);
}

int hashtable_has(struct hashtable *t, char *key) {
        int index = hash(key) % t->size, currIndex = index;
        while (1) {
                if (t->table[index].key &&
                    strcmp(t->table[index].key, key) == 0)
                        return 1;

                currIndex = (currIndex + 1) % t->size;

                if (currIndex == index) return 0;
        }
}
