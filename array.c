//
// Created by Teppo Koskinen on 13/06/15.
//

#include <stdlib.h>
#include "array.h"
#include "memory.h"
#include "interpreter.h"

#define SIZEOF(len) ((unsigned int) (sizeof(Array) + len * sizeof(REF)))

REF array_create(unsigned int size, FLAGS type) {
    REF ref = mem_alloc(SIZEOF(size), type);

    Array *array = HANDLES.slot[ref].data;
    array->len = size;
    array->capacity = size;

    return ref;
}

void array_map(CALLBACK map, REF ref) {
    Array *array = HANDLES.slot[ref].data;

    int i = 0;
    while(i < array->len) {
        map(array->ref[i++]);
    }
}

REF *array_ptr_to_index(REF ref, int index) {
    Handle *handle = &HANDLES.slot[ref];
    Array *array = handle->data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        return ILLEGAL_REF;
    }

    return &(array->ref[index]);
}

REF array_get(REF ref, int index) {
    return *array_ptr_to_index(ref, index);
}

void array_set(REF ref, int index, REF value) {
    REF *item = array_ptr_to_index(ref, index);
    if(item == ILLEGAL_REF) {
        recover("Array index out of bounds\0");
    }

    *item = value;
}

void array_del(REF ref, int index) {
    Array *array = HANDLES.slot[ref].data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        recover("Array index out of bounds\0");
    }

    array->len--;

    while(index < array->len) {
        array->ref[index++] = array->ref[index+1];
    }
}

void array_ensure_capacity(REF ref) {
    Array *array = HANDLES.slot[ref].data;

    if(array->len == array->capacity) {
        array->capacity += 10;
        mem_realloc(ref, SIZEOF(array->capacity));
    }
}

void array_append(REF ref, REF item) {
    array_ensure_capacity(ref);

    Array *array = HANDLES.slot[ref].data;
    array->ref[array->len] = item;
    array->len++;
}
