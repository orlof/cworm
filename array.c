//
// Created by Teppo Koskinen on 13/06/15.
//

#include "array.h"
#include "interpreter.h"
#include "stdutil.h"

#define SIZEOF(len) ((unsigned int) (sizeof(Array) + len * sizeof(REF)))

// private declarations

static Array *array_ensure_capacity(REF myref);
static int normalize_index(REF myref, int index);

// public interface

REF array_create(unsigned int size, FLAGS type) {
    REF result = mem_alloc(SIZEOF(size), type);

    Array *array = HANDLES.slot[result].data;
    array->len = size;
    array->capacity = size;

    return result;
}

REF array_split(REF myref, int start, int end) {
    start = normalize_index(myref, start);
    end = normalize_index(myref, end);

    if(start > end) {
        start = end;
    }

    int len = end - start;
    REF result = array_create((unsigned int) len, HANDLES.slot[myref].flags);

    Array *src = HANDLES.slot[myref].data;
    Array *dst = HANDLES.slot[result].data;

    int i;
    for(i = 0; i < len; i++) {
        dst->ref[i] = src->ref[start + i];
    }

    return result;
}

REF array_merge(REF leftref, REF rightref) {
    Array *left = HANDLES.slot[leftref].data;
    Array *right = HANDLES.slot[rightref].data;

    REF resultref = array_create(left->len + right->len, HANDLES.slot[leftref].flags);

    left = HANDLES.slot[leftref].data;
    right = HANDLES.slot[rightref].data;
    Array *result = HANDLES.slot[resultref].data;

    int i;
    for(i = 0; i < left->len; i++) {
        result->ref[i] = left->ref[i];
    }

    int j;
    for(j = 0; j < right->len; j++, i++) {
        result->ref[i] = right->ref[j];
    }

    return resultref;
}

REF array_repeat(REF baseref, unsigned int mult) {
    Array *basearr = HANDLES.slot[baseref].data;

    REF resref = array_create(basearr->len * mult, HANDLES.slot[baseref].flags);

    basearr = HANDLES.slot[baseref].data;
    Array *resarr = HANDLES.slot[resref].data;

    int i;
    for(i = 0; i < resarr->len; i++) {
        resarr->ref[i] = basearr->ref[i % basearr->len];
    }

    return resref;
}

// other

void array_map(REF myref, CALLBACK map) {
    Array *array = HANDLES.slot[myref].data;

    int i = 0;
    while(i < array->len) {
        map(array->ref[i++]);
    }
}

REF array_get(REF myref, int index) {
    Handle *handle = &HANDLES.slot[myref];
    Array *array = handle->data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        return ILLEGAL_REF;
    }

    return array->ref[index];
}

int array_cmp(REF lref, REF rref) {
    Array *larr = HANDLES.slot[lref].data;
    Array *rarr = HANDLES.slot[rref].data;

    int i;
    for(i = 0; i < larr->len && i < rarr->len; i++) {
        int result = val_cmp(larr->ref[i], rarr->ref[i]);
        if(result != 0) {
            return result;
        }
    }

    // sign(llen - rlen)
    return (larr->len > rarr->len) - (larr->len < rarr->len);
}

void array_set(REF myref, int index, REF value) {
    Handle *handle = &HANDLES.slot[myref];
    Array *array = handle->data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        recover("Array index out of bounds\0");
    }

    array->ref[index] = value;
}

void array_del(REF myref, int index) {
    Array *array = HANDLES.slot[myref].data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0 || index >= array->len) {
        recover("Array index out of bounds\0");
    }

    array->len--;

    for( /**/; index < array->len; index++) {
        array->ref[index] = array->ref[index+1];
    }
}

void array_append(REF myref, REF item) {
    Array *array = array_ensure_capacity(myref);

    array->ref[array->len] = item;
    array->len++;
}

void array_append_array(REF myref, REF otherref) {
    Array *otherarr = HANDLES.slot[otherref].data;
    int otherlen = otherarr->len;

    int i;
    for(i = 0; i < otherlen; i++) {
        array_append(myref, array_get(otherref, i));
    }
}

void array_insert(REF myref, int index, REF item) {
    index = normalize_index(myref, index);

    Array *array = array_ensure_capacity(myref);

    int i;
    for(i = array->len; i > index; i--) {
        array->ref[i] = array->ref[i-1];
    }

    array->len++;

    array->ref[index] = item;
}

// local functions

static Array *array_ensure_capacity(REF myref) {
    Array *array = HANDLES.slot[myref].data;

    if(array->len == array->capacity) {
        array->capacity += 10;
        mem_realloc(myref, SIZEOF(array->capacity));
    }

    return HANDLES.slot[myref].data;
}

static int normalize_index(REF myref, int index) {
    Array *array = HANDLES.slot[myref].data;

    if(index < 0) {
        index += array->len;
    }

    if(index < 0) {
        index = 0;
    }

    if(index > array->len) {
        index = array->len;
    }

    return index;
}
