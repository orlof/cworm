//
// Created by Teppo Koskinen on 13/06/15.
//

#include <stdlib.h>
#include "array.h"
#include "memory.h"

typedef struct Array Array;

struct Array {
    unsigned int len;
    HANDLE items[];
};

HANDLE *array_create(unsigned int size, unsigned int type) {
    return mem_alloc(sizeof(Array) + size * sizeof(HANDLE *), type);
}

void array_map(HANDLE_PROCESSOR map, HANDLE *handle) {
    Array *array = handle->data;

    for(int i=0; i < array->len; i++) {
        map(&array->items[i]);
    }
}

HANDLE *array_get_item_ptr_by_index(HANDLE *handle, int index) {
    Array *array = handle->data;
    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        return 0;
    }

    return &(array->items[index]);
}