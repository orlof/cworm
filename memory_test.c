#include <stdlib.h>
#include "minunit.h"
#include "memory.h"

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

    mem_clear(mem+50, mem+53);
    mu_assert("mem_clear 1/1", mem[49]==49);
    mu_assert("mem_clear 1/2", mem[50]==0);
    mu_assert("mem_clear 1/3", mem[51]==0);
    mu_assert("mem_clear 1/4", mem[52]==0);
    mu_assert("mem_clear 1/5", mem[53]==53);

    free(mem);
    return 0;
}

static char * test_call_stack() {
    mem_initialize();

    mu_assert("test_init 1", STACK.sp == STACK_SIZE);

    call_stack_push(1);

    mu_assert("test_init 2.1", STACK.sp == STACK_SIZE-1);
    mu_assert("test_init 2.2", STACK.slot[STACK.sp] == 1);

    call_stack_push(2);

    mu_assert("test_init 3.1", STACK.sp == STACK_SIZE-2);
    mu_assert("test_init 3.2", STACK.slot[STACK.sp] == 2);

    REF ref1 = call_stack_pop();

    mu_assert("test_init 4.0", ref1 == 2);
    mu_assert("test_init 4.1", STACK.sp == STACK_SIZE-1);
    mu_assert("test_init 4.2", STACK.slot[STACK.sp] == 1);

    REF ref2 = call_stack_pop();

    mu_assert("test_init 5.0", ref2 == 1);
    mu_assert("test_init 5.1", STACK.sp == STACK_SIZE);

    mem_destroy();
    return 0;
}

#define BIG (HEAP_SIZE/2+10)

static char * test_alloc() {
    mem_initialize();
    REF ref1 = mem_alloc(BIG, INT);

    mu_assert("test_alloc 111", HEAP.free == HEAP.mem + BIG);

    mu_assert("test_alloc 120", ref1 == 0);
    mu_assert("test_alloc 121", HANDLES.resv_head == ref1);
    mu_assert("test_alloc 122", HANDLES.resv_tail == ref1);
    mu_assert("test_alloc 123", HANDLES.free_head == 1);

    Handle *h1 = &HANDLES.slot[ref1];
    mu_assert("test_alloc 130", h1->data == (void *) HEAP.mem);
    mu_assert("test_alloc 131", h1->size == BIG);
    mu_assert("test_alloc 132", h1->flags == INT);
    mu_assert("test_alloc 133", h1->next == HANDLES_SIZE);

    REF ref2 = mem_alloc(5, STR);

    mu_assert("test_alloc 211", HEAP.free == HEAP.mem + BIG + 5);

    mu_assert("test_alloc 2200", ref1 == 0);
    mu_assert("test_alloc 2201", ref2 == 1);
    mu_assert("test_alloc 221", HANDLES.resv_head == ref1);
    mu_assert("test_alloc 222", HANDLES.resv_tail == ref2);
    mu_assert("test_alloc 223", HANDLES.free_head == 2);

    mu_assert("test_alloc 230", h1->data == (void *) HEAP.mem);
    mu_assert("test_alloc 231", h1->size == BIG);
    mu_assert("test_alloc 232", h1->flags == INT);
    mu_assert("test_alloc 233", h1->next == ref2);

    Handle *h2 = &HANDLES.slot[ref2];
    mu_assert("test_alloc 230", h2->data == (void *) HEAP.mem + BIG);
    mu_assert("test_alloc 231", h2->size == 5);
    mu_assert("test_alloc 232", h2->flags == STR);
    mu_assert("test_alloc 233", h2->next == HANDLES_SIZE);

    call_stack_push(ref2);

    REF ref3 = mem_alloc(BIG, FLOAT);

    mu_assert("test_alloc 311", HEAP.free == HEAP.mem + BIG + 5);

    mu_assert("test_alloc 2200", ref3 == 0);
    mu_assert("test_alloc 2201", ref2 == 1);
    mu_assert("test_alloc 321", HANDLES.resv_head == ref2);
    mu_assert("test_alloc 322", HANDLES.resv_tail == ref3);
    mu_assert("test_alloc 323", HANDLES.free_head == 2);

    Handle *h3 = &HANDLES.slot[ref3];
    mu_assert("test_alloc 330", h3->data == (void *) (HEAP.mem + 5));
    mu_assert("test_alloc 331", h3->size == BIG);
    mu_assert("test_alloc 332", h3->flags == FLOAT);
    mu_assert("test_alloc 333", h3->next == HANDLES_SIZE);

    mu_assert("test_alloc 340", h2->data == (void *) (HEAP.mem));
    mu_assert("test_alloc 341", h2->size == 5);
    mu_assert("test_alloc 342", h2->flags == STR);
    mu_assert("test_alloc 343", h2->next == ref3);

    mem_collect_garbage();

    mu_assert("test_alloc 411", HEAP.free == HEAP.mem + 5);

    mu_assert("test_alloc 4201", ref2 == 1);
    mu_assert("test_alloc 421", HANDLES.resv_head == ref2);
    mu_assert("test_alloc 422", HANDLES.resv_tail == ref2);
    mu_assert("test_alloc 423", HANDLES.free_head == 0);

    mu_assert("test_alloc 433", h2->next == HANDLES_SIZE);
    mu_assert("test_alloc 433", HANDLES.slot[0].next == 2);


    call_stack_pop();
    mem_collect_garbage();

    mu_assert("test_alloc 511", HEAP.free == HEAP.mem);

    mu_assert("test_alloc 521", HANDLES.resv_head == HANDLES_SIZE);
    mu_assert("test_alloc 522", HANDLES.resv_tail == HANDLES_SIZE);
    mu_assert("test_alloc 523", HANDLES.free_head == 1);

    mu_assert("test_alloc 533", HANDLES.slot[0].next == 2);
    mu_assert("test_alloc 534", HANDLES.slot[1].next == 0);
    mu_assert("test_alloc 535", HANDLES.slot[2].next == 3);

    REF ref4 = mem_alloc(5, STR);
    REF ref5 = mem_alloc(5, STR);

    mu_assert("test_alloc 611", HEAP.free == HEAP.mem+10);

    mu_assert("test_alloc 612", ref4 == 1);
    mu_assert("test_alloc 613", ref5 == 0);

    mu_assert("test_alloc 621", HANDLES.resv_head == 1);
    mu_assert("test_alloc 622", HANDLES.resv_tail == 0);
    mu_assert("test_alloc 623", HANDLES.free_head == 2);

    mu_assert("test_alloc 633", HANDLES.slot[0].next == HANDLES_SIZE);
    mu_assert("test_alloc 634", HANDLES.slot[1].next == 0);
    mu_assert("test_alloc 635", HANDLES.slot[2].next == 3);

    REF ref6 = mem_alloc(5, STR);

    mu_assert("test_alloc 711", HEAP.free == HEAP.mem+15);

    mu_assert("test_alloc 612", ref4 == 1);
    mu_assert("test_alloc 613", ref5 == 0);
    mu_assert("test_alloc 614", ref6 == 2);

    mu_assert("test_alloc 721", HANDLES.resv_head == 1);
    mu_assert("test_alloc 722", HANDLES.resv_tail == 2);
    mu_assert("test_alloc 723", HANDLES.free_head == 3);

    mu_assert("test_alloc 733", HANDLES.slot[0].next == 2);
    mu_assert("test_alloc 734", HANDLES.slot[1].next == 0);
    mu_assert("test_alloc 735", HANDLES.slot[2].next == HANDLES_SIZE);
    mu_assert("test_alloc 736", HANDLES.slot[3].next == 4);

    mem_destroy();
    return 0;
}

char * test_memory() {
    mu_run_test(test_mem_utilities);
    mu_run_test(test_call_stack);
    mu_run_test(test_alloc);

    return 0;
}
