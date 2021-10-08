#ifndef RESOLVE_TYPES_H
#define RESOLVE_TYPES_H

#include "hashtable/hash_table.h"
#include "hashtable/hashtable_types.h"

struct Scope {
        struct hashtable* h;
        struct Scope* next;
        struct Scope* prev;
};
#endif