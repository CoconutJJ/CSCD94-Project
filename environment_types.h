
#ifndef ENV_TYPE_H
#define ENV_TYPE_H
#include "./hashtable/hash_table.h"

struct Environment {
        struct hashtable * values;
};

#endif
