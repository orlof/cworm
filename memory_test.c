/* file minunit_example.c */
 
#include <stdio.h>
#include "minunit.h"
#include "memory.h"

#define MEM 80

int tests_run = 0;

static char * test_init() {
    init(MEM);
    mu_assert("test_init 1", HW.mem > 0);
    mu_assert("test_init 2", (void *) HW.mem + MEM == (void *) HW.sp);
    return 0;
}

static char * test_push_handle() {
    push(HW.mem);

    mu_assert("test_push_handle 1", (void *) HW.mem + MEM - sizeof(void *) == (void *) HW.sp);
    mu_assert("test_push_handle 2", (void *) HW.mem == *HW.sp);
    return 0;
}

static char * test_push_frame() {
    push(HW.sp);

    mu_assert("test_push_frame 1", (void *) HW.mem + MEM - 2 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_push_frame 2", (void *) HW.mem + MEM - 1 * sizeof(void *) == (void *) *HW.sp);
    return 0;
}

static char * test_pop_frame() {
    void *fp = pop();

    mu_assert("test_pop_frame 1", (void *) HW.mem + MEM - 1 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_pop_frame 2", fp == HW.sp);
    return 0;
}

static char * test_pop_handle() {
    void *h = pop();

    mu_assert("test_pop_handle 1", (void *) HW.mem + MEM - 0 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_pop_handle 2", h == HW.mem);
    return 0;
}



static char * all_tests() {
    mu_run_test(test_init);
    mu_run_test(test_push_handle);
    mu_run_test(test_push_frame);
    mu_run_test(test_pop_frame);
    mu_run_test(test_pop_handle);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
