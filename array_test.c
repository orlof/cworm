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

    array_map(ref1, count);

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

static char * test_array_insert() {
    mem_initialize();
    REF ref1 = array_create(3, LIST);

    Array *arr1 = HANDLES.slot[ref1].data;
    arr1->len = 0;

    Handle *h1 = &HANDLES.slot[ref1];
    array_insert(ref1, 0, 10);
    array_insert(ref1, 1, 11);
    array_insert(ref1, 2, 12);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_insert*3 handle size", h1->size == sizeof(Array) + 3*sizeof(REF));
    mu_assert("test_array insert*3 len and cap", arr->len==3 && arr->capacity == 3);
    mu_assert("test_array_insert*3 items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12);

    array_insert(ref1, -1, 13);
    arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_insert-1 handle size", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array insert-1 len and cap", arr->len==4 && arr->capacity == 13);
    mu_assert("test_array_insert-1 items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==13 && arr->ref[3]==12);

    array_insert(ref1, -10, 14);
    arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_insert-1 handle size", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array insert-1 len and cap", arr->len==5 && arr->capacity == 13);
    mu_assert("test_array_insert-1 items", arr->ref[0]==14 && arr->ref[1]==10 && arr->ref[2]==11 && arr->ref[3]==13 && arr->ref[4]==12);

    array_insert(ref1, 10, 15);
    arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_insert-1 handle size", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array insert-1 len and cap", arr->len==6 && arr->capacity == 13);
    mu_assert("test_array_insert-1 items", arr->ref[0]==14 && arr->ref[1]==10 && arr->ref[2]==11 && arr->ref[3]==13 && arr->ref[4]==12 && arr->ref[5]==15);

    mem_destroy();
    return 0;
}

static char * test_array_append_array() {
    mem_initialize();
    REF ref1 = array_create(3, LIST);
    REF ref2 = array_create(3, LIST);

    Array *arr1 = HANDLES.slot[ref1].data;
    Array *arr2 = HANDLES.slot[ref2].data;
    arr1->len = 0;
    arr2->len = 0;

    Handle *h1 = &HANDLES.slot[ref1];
    array_append(ref1, 10);
    array_append(ref1, 11);
    array_append(ref2, 12);
    array_append(ref2, 13);

    array_append_array(ref1, ref2);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_append_array handle size", h1->size == sizeof(Array) + 13*sizeof(REF));
    mu_assert("test_array_append_array len and cap", arr->len==4 && arr->capacity == 13);
    mu_assert("test_array_append_array items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==13);

    mem_destroy();
    return 0;
}

static char * test_array_split() {
    mem_initialize();
    REF ref1 = array_create(4, LIST);

    array_set(ref1, 0, 10);
    array_set(ref1, 1, 11);
    array_set(ref1, 2, 12);
    array_set(ref1, 3, 13);

    REF ref2 = array_split(ref1, 0, 0);

    Array *arr = HANDLES.slot[ref2].data;
    mu_assert("test_array_split0 handle size", HANDLES.slot[ref2].size == sizeof(Array) + 0 * sizeof(REF));
    mu_assert("test_array_split0 len and cap", arr->len==0 && arr->capacity == 0);
    // mu_assert("test_array_split items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==13);

    ref2 = array_split(ref1, 0, 4);
    arr = HANDLES.slot[ref2].data;
    mu_assert("test_array_split04 handle size", HANDLES.slot[ref2].size == sizeof(Array) + 4 * sizeof(REF));
    mu_assert("test_array_split04 len and cap", arr->len==4 && arr->capacity == 4);
    mu_assert("test_array_split04 items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==13);

    ref2 = array_split(ref1, -2, -1);
    arr = HANDLES.slot[ref2].data;
    mu_assert("test_array_split-2-1 handle size", HANDLES.slot[ref2].size == sizeof(Array) + 1 * sizeof(REF));
    mu_assert("test_array_split-2-1 len and cap", arr->len==1 && arr->capacity == 1);
    mu_assert("test_array_split-2-1 items", arr->ref[0]==12);

    ref2 = array_split(ref1, -1, 10);
    arr = HANDLES.slot[ref2].data;
    mu_assert("test_array_split-110 handle size", HANDLES.slot[ref2].size == sizeof(Array) + 1 * sizeof(REF));
    mu_assert("test_array_split-110 len and cap", arr->len==1 && arr->capacity == 1);
    mu_assert("test_array_split-110 items", arr->ref[0]==13);

    mem_destroy();
    return 0;
}

static char * test_array_merge() {
    mem_initialize();
    REF ref1 = array_create(2, LIST);
    REF ref2 = array_create(2, LIST);

    array_set(ref1, 0, 10);
    array_set(ref1, 1, 11);

    array_set(ref2, 0, 12);
    array_set(ref2, 1, 13);

    REF ref3 = array_merge(ref1, ref2);

    Array *arr = HANDLES.slot[ref3].data;
    mu_assert("test_array_merge handle size", HANDLES.slot[ref3].size == sizeof(Array) + 4 * sizeof(REF));
    mu_assert("test_array_merge len and cap", arr->len==4 && arr->capacity == 4);
    mu_assert("test_array_merge items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==12 && arr->ref[3]==13);

    mem_destroy();
    return 0;
}

static char * test_array_merge2() {
    mem_initialize();
    REF ref1 = array_create(0, LIST);
    REF ref2 = array_create(0, LIST);

    REF ref3 = array_merge(ref1, ref2);

    Array *arr = HANDLES.slot[ref3].data;
    mu_assert("test_array_merge handle size", HANDLES.slot[ref3].size == sizeof(Array) + 0 * sizeof(REF));
    mu_assert("test_array_merge len and cap", arr->len==0 && arr->capacity == 0);

    mem_destroy();
    return 0;
}

static char * test_array_repeat() {
    mem_initialize();
    REF ref1 = array_create(2, LIST);

    array_set(ref1, 0, 10);
    array_set(ref1, 1, 11);

    REF ref3 = array_repeat(ref1, 2);

    Array *arr = HANDLES.slot[ref3].data;
    mu_assert("test_array_merge handle size", HANDLES.slot[ref3].size == sizeof(Array) + 4 * sizeof(REF));
    mu_assert("test_array_merge len and cap", arr->len==4 && arr->capacity == 4);
    mu_assert("test_array_merge items", arr->ref[0]==10 && arr->ref[1]==11 && arr->ref[2]==10 && arr->ref[3]==11);

    mem_destroy();
    return 0;
}

static char * test_array_repeat2() {
    mem_initialize();
    REF ref1 = array_create(2, LIST);

    array_set(ref1, 0, 10);
    array_set(ref1, 1, 11);

    REF ref3 = array_repeat(ref1, 0);

    Array *arr = HANDLES.slot[ref3].data;
    mu_assert("test_array_merge handle size", HANDLES.slot[ref3].size == sizeof(Array) + 0 * sizeof(REF));
    mu_assert("test_array_merge len and cap", arr->len==0 && arr->capacity == 0);

    mem_destroy();
    return 0;
}

static char * test_array_sort() {
    mem_initialize();
    REF ref1 = array_create(4, LIST);

    array_set(ref1, 0, 14);
    array_set(ref1, 1, 12);
    array_set(ref1, 2, 11);
    array_set(ref1, 3, 14);

    array_sort(ref1, 0);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_sort ascending handle", HANDLES.slot[ref1].size == sizeof(Array) + 4 * sizeof(REF));
    mu_assert("test_array_sort ascending array", arr->len==4 && arr->capacity == 4);
    mu_assert("test_array_sort ascending items", arr->ref[0]==11 && arr->ref[1]==12 && arr->ref[2]==14 && arr->ref[3]==14);

    mem_destroy();
    return 0;
}

static char * test_array_sort_desc() {
    mem_initialize();
    REF ref1 = array_create(4, LIST);

    array_set(ref1, 0, 14);
    array_set(ref1, 1, 12);
    array_set(ref1, 2, 11);
    array_set(ref1, 3, 14);

    array_sort(ref1, 0);

    Array *arr = HANDLES.slot[ref1].data;
    mu_assert("test_array_sort descending handle", HANDLES.slot[ref1].size == sizeof(Array) + 4 * sizeof(REF));
    mu_assert("test_array_sort descending array", arr->len==4 && arr->capacity == 4);
    mu_assert("test_array_sort descending items", arr->ref[0]==14 && arr->ref[1]==14 && arr->ref[2]==12 && arr->ref[3]==11);

    mem_destroy();
    return 0;
}

char * test_array() {
    mu_run_test(test_array_create);
    mu_run_test(test_array_get);
    mu_run_test(test_array_insert);
    mu_run_test(test_array_append_array);
    mu_run_test(test_array_split);
    mu_run_test(test_array_merge);
    mu_run_test(test_array_merge2);
    mu_run_test(test_array_repeat);
    mu_run_test(test_array_repeat2);
    //mu_run_test(test_array_sort);
    //mu_run_test(test_array_sort_desc);

    return 0;
}
