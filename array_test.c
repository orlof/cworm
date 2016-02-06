#include <stdio.h>
#include "minunit.h"
#include "array.h"

int counter=0;
void count(REF ref) {
    counter++;
}

static char * test_array_create() {
    mem_initialize();
    REF ref1 = array_create(5, LIST);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_create initial size set to capacity", arr->len==5 && arr->capacity == 5);

    Handle *h1 = &HANDLES.slot[ref1];
    mu_assert("test_array_create 111", h1->size == sizeof(Array) + 5*sizeof(REF));
    mu_assert("test_array_create 112", ((Array *) h1->data)->len == 5);

    array_map(count, ref1);

    mu_assert("test_array_map 222", counter == 5);

    mem_destroy();
    return 0;
}

static char * test_array_get() {
    mem_initialize();
    REF ref1 = array_create(3, LIST);

    Array *arr1 = HANDLES.slot[ref1].data;
    arr1->len = 0;

    Handle *h1 = &HANDLES.slot[ref1];
    array_append(ref1, 10);
    array_append(ref1, 11);
    array_append(ref1, 12);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_append*3 handle size", h1->size == sizeof(Array) + 3*sizeof(REF));
    mu_assert("test_array append*3 len and cap", arr->len==3 && arr->capacity == 3);
    mu_assert("test_array_append*3 items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12);

    array_append(ref1, 13);
    arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_append*4 handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array append*4 len and cap", arr->len==4 && arr->capacity == 13);
    mu_assert("test_array_append*4 items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==13);

    array_set(ref1, 0, 100);
    array_set(ref1, 3, 1000);
    mu_assert("test_array_set handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array set len and cap", arr->len==4 && arr->capacity == 13);
    mu_assert("test_array_set items", arr->ref[0]==100 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==1000);

    mu_assert("test_array_get0", array_get(ref1, 0) == 100);
    mu_assert("test_array_get-1", array_get(ref1, -1) == 1000);
    mu_assert("test_array_get3", array_get(ref1, 3) == 1000);
    mu_assert("test_array_get-4", array_get(ref1, -4) == 100);

    array_del(ref1, 3);
    mu_assert("test_array_del3 handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array_del3 len and cap", arr->len==3 && arr->capacity == 13);
    mu_assert("test_array_del3 items", arr->ref[0]==100 && arr->ref[1]==11 && arr->ref[2]==12);

    array_del(ref1, -1);
    mu_assert("test_array_del-1 handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array_del-1 len and cap", arr->len==2 && arr->capacity == 13);
    mu_assert("test_array_del-1 items", arr->ref[0]==100 && arr->ref[1]==11);

    array_del(ref1, -2);
    mu_assert("test_array_del-2 handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array_del-2 len and cap", arr->len==1 && arr->capacity == 13);
    mu_assert("test_array_del-2 items", arr->ref[0]==11);

    array_del(ref1, 0);
    mu_assert("test_array_del0 handle", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array_del0 len and cap", arr->len==0 && arr->capacity == 13);

    mem_destroy();
    return 0;
}



char * test_array() {
    mu_run_test(test_array_create);
    mu_run_test(test_array_get);

    return 0;
}
