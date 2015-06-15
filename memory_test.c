/* file minunit_example.c */
 
#include <stdio.h>
#include "minunit.h"
#include "memory.h"
 
int tests_run = 0;
 
static char * test_init() {
    init(10);
    mu_assert("HW.mem == 0", HW.mem > 0);
    mu_assert("HW.mem+10 == HW.sp", (void *) HW.mem+10 == (void *) HW.sp);
    return 0;
}

static char * test_push_handle() {
    push(HW.mem);

    mu_assert("HW.mem+6 == HW.sp", (void *) HW.mem+6 == (void *) HW.sp);
    mu_assert("HW.mem == *HW.sp", (void *) HW.mem == (void *) *HW.sp);
    return 0;
}

static char * test_push_frame() {
    push(HW.sp);

    mu_assert("HW.mem+2 == HW.sp", (void *) HW.mem+2 == (void *) HW.sp);
    mu_assert("HW.mem+6 == *HW.sp", (void *) HW.mem+6 == (void *) *HW.sp);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_init);
    mu_run_test(test_push_handle);
    mu_run_test(test_push_frame);
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
