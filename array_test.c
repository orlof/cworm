#include <stdio.h>
#include "minunit.h"
#include "array.h"

int counter=0;
void count(REF ref) {
    printf("CALLING\n");
    counter++;
}

static char * test_array_create() {
    mem_initialize();
    REF ref1 = array_create(5, LIST);

    Handle *h1 = &HANDLES.slot[ref1];
    mu_assert("test_array_create 111", h1->size == sizeof(Array) + 5*sizeof(REF));
    mu_assert("test_array_create 112", ((Array *) h1->data)->len == 5);

    array_map(count, ref1);

    mu_assert("test_array_map 222", counter == 5);

    mem_destroy();
    return 0;
}

char * test_array() {
    mu_run_test(test_array_create);

    return 0;
}
