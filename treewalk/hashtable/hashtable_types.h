
#ifndef HASHTABLE_TYPES_H
#define HASHTABLE_TYPES_H
struct hashcontainer {
        char *key;
        void *item;
        int deleted;
};

struct hashtable {
        struct hashcontainer *table;
        int size;
        int filled;
};
#endif