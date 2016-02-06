//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_ARRAY_H
#define WORM_ARRAY_H

#include "memory.h"

typedef struct Array Array;

struct Array {
    unsigned int len;
    REF ref[];
};

typedef void (*CALLBACK)(REF);

REF array_create(unsigned int size, FLAGS type);
void array_map(CALLBACK map, REF ref);

#endif //WORM_ARRAY_H
