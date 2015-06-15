//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_ARRAY_H
#define WORM_ARRAY_H

#include "memory.h"

typedef void (*map_func)(HANDLE *);

void array_create(unsigned int size, unsigned int type);
void array_map(map_func func, HANDLE *handle);

#endif //WORM_ARRAY_H
