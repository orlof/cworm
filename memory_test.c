/* file minunit_example.c */
 
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "memory.h"

#define MEM 80

int tests_run = 0;

static char * test_mem_utilities() {
    char *mem = malloc(100);

    unsigned char i;
    for(i=0; i < 100; i++) {
        mem[i] = i;
    }

    mem_copy(mem+2, 5, mem+1, 5);
    mu_assert("mem_copy 1/0", mem[0]==0);
    mu_assert("mem_copy 1/1", mem[1]==2);
    mu_assert("mem_copy 1/2", mem[2]==3);
    mu_assert("mem_copy 1/3", mem[3]==4);
    mu_assert("mem_copy 1/4", mem[4]==5);
    mu_assert("mem_copy 1/5", mem[5]==6);
    mu_assert("mem_copy 1/6", mem[6]==6);

    mem_copy(mem+20, 3, mem+10,5);
    mu_assert("mem_copy 2/0", mem[9]==9);
    mu_assert("mem_copy 2/1", mem[10]==20);
    mu_assert("mem_copy 2/2", mem[11]==21);
    mu_assert("mem_copy 2/3", mem[12]==22);
    mu_assert("mem_copy 2/4", mem[13]==0);
    mu_assert("mem_copy 2/5", mem[14]==0);
    mu_assert("mem_copy 2/6", mem[15]==15);

    mem_clear(mem+50, 3);
    mu_assert("mem_clear 1/1", mem[49]==49);
    mu_assert("mem_clear 1/2", mem[50]==0);
    mu_assert("mem_clear 1/3", mem[51]==0);
    mu_assert("mem_clear 1/4", mem[52]==0);
    mu_assert("mem_clear 1/5", mem[53]==53);

    free(mem);
    return 0;
}

static char * test_call_stack() {
    mem_initialize(60, 20);

    mu_assert("test_init 1", HW.mem > 0);
    mu_assert("test_init 2", (void *) HW.mem + MEM == (void *) HW.sp);

    call_stack_push(HW.mem);

    mu_assert("test_push_handle 1", (void *) HW.mem + MEM - sizeof(void *) == (void *) HW.sp);
    mu_assert("test_push_handle 2", (void *) HW.mem == *HW.sp);

    call_stack_push(HW.sp);

    mu_assert("test_push_frame 1", (void *) HW.mem + MEM - 2 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_push_frame 2", (void *) HW.mem + MEM - 1 * sizeof(void *) == (void *) *HW.sp);

    void *fp = call_stack_pop();

    mu_assert("test_pop_frame 1", (void *) HW.mem + MEM - 1 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_pop_frame 2", fp == HW.sp);

    void *h = call_stack_pop();

    mu_assert("test_pop_handle 1", (void *) HW.mem + MEM - 0 * sizeof(void *) == (void *) HW.sp);
    mu_assert("test_pop_handle 2", h == HW.mem);

    mem_destroy();
    return 0;
}

static char * test_alloc() {
    mem_initialize(60, 20);
    HANDLE *h = mem_alloc(10, 5);

    mu_assert("test_alloc 1", HW.mem_stack == (HW.mem_handle + sizeof(HANDLE)));
    mu_assert("test_alloc 1.0", h == (HANDLE *) HW.mem_handle);
    mu_assert("test_alloc 1.1", HW.mem + 10 == HW.mem_free);
    mu_assert("test_alloc 1.2", HW.handle_resv_head == h);
    mu_assert("test_alloc 1.3", HW.handle_resv_tail == h);
    mu_assert("test_alloc 2", h->data == (void *) HW.mem);
    mu_assert("test_alloc 3", h->size == 10);
    mu_assert("test_alloc 4", h->type == 5);
    mu_assert("test_alloc 5", h->next == 0);

    HANDLE *h2 = mem_alloc(5, 5);

    mu_assert("test_alloc 21", HW.mem_stack == (HW.mem_handle + 2*sizeof(HANDLE)));
    mu_assert("test_alloc 21.0", h2 == (HANDLE *) HW.mem_handle);
    mu_assert("test_alloc 21.1", HW.mem + 15 == HW.mem_free);
    mu_assert("test_alloc 21.2", HW.handle_resv_head == h);
    mu_assert("test_alloc 21.3", HW.handle_resv_tail == h2);
    mu_assert("test_alloc 22", h2->data == (void *) (HW.mem+10));
    mu_assert("test_alloc 23", h2->size == 5);
    mu_assert("test_alloc 24", h2->type == 5);
    mu_assert("test_alloc 25", h2->next == 0);
    mu_assert("test_alloc 25", h->next == h2);

    call_stack_push(HW.sp);
    call_stack_push(h2);
    call_stack_push(HW.sp);

    mem_destroy();
    return 0;
}

static char * all_tests() {
//    mu_run_test(test_mem_utilities);
//    mu_run_test(test_call_stack);
    mu_run_test(test_alloc);

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
