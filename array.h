//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_ARRAY_H
#define WORM_ARRAY_H

#include "memory.h"

typedef void (*HANDLE_PROCESSOR)(HANDLE *);

HANDLE *array_create(unsigned int size, unsigned int type);
void array_map(HANDLE_PROCESSOR map, HANDLE *handle);

#endif //WORM_ARRAY_H
