//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_ARRAY_H
#define WORM_ARRAY_H

#include "memory.h"

typedef struct Array Array;

struct Array {
    unsigned int len;
    unsigned int capacity;
    REF ref[];
};

typedef void (*CALLBACK)(REF);

// these create new array
REF array_create(unsigned int size, FLAGS type);
REF array_split(REF myref, int start, int end);
REF array_merge(REF left, REF right);
REF array_repeat(REF base, unsigned int mult);

// these modify myref
void array_set(REF myref, int index, REF value);
void array_del(REF myref, int index);
void array_append(REF myref, REF item);
void array_append_array(REF myref, REF otherref);
void array_insert(REF myref, int index, REF item);

// others
REF array_get(REF myref, int index);
void array_map(REF myref, CALLBACK map);
int array_cmp(REF left, REF right);

#endif //WORM_ARRAY_H
