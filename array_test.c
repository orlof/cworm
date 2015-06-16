/* file minunit_example.c */
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "array.h"

int tests_run = 0;

static char * test_pop_handle() {
    mu_assert("test_pop_handle 1", (void *) HW.mem + MEM - 0 * sizeof(void *) == (void *) HW.sp);
    return 0;
}



static char * all_tests() {
    mu_run_test(test_mem_copy);
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
