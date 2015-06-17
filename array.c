//
// Created by Teppo Koskinen on 13/06/15.
//

#include <stdlib.h>
#include "array.h"
#include "memory.h"

HANDLE *array_create(unsigned int size, unsigned int type) {
    HANDLE *handle = mem_alloc((unsigned int) (sizeof(Array) + size * sizeof(HANDLE *)), type);
    ((Array *) handle->data)->len = size;
    return handle;
}

void array_map(HANDLE_PROCESSOR map, HANDLE *handle) {
    Array *array = handle->data;

    int i;
    for(i=0; i < array->len; i++) {
        map(array->items[i]);
    }
}

HANDLE **array_get_item_ptr_by_index(HANDLE *handle, int index) {
    Array *array = handle->data;
    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        return 0;
    }

    return &(array->items[index]);
}
