//
// Created by Teppo Koskinen on 13/06/15.
//

#include <stdlib.h>
#include "array.h"
#include "memory.h"

#define SIZEOF(len) ((unsigned int) (sizeof(Array) + len * sizeof(REF)))

REF array_create(unsigned int size, FLAGS type) {
    REF ref = mem_alloc(SIZEOF(size), type);
    ((Array *) HANDLES.slot[ref].data)->len = size;

    return ref;
}

void array_map(CALLBACK map, REF ref) {
    Array *array = HANDLES.slot[ref].data;

    int i;
    for(i=0; i < array->len; i++) {
        map(array->ref[i]);
    }
}

/*
REF *array_get_item_ptr_by_index(REF ref, int index) {
    Handle *handle = &HANDLES.slot[ref];
    Array *array = handle->data;
    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        return HANDLES_SIZE;
    }

    return &(array->ref[index]);
}
*/