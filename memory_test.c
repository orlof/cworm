
#define MEM 80

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
    mem_initialize(200, 100);
    HANDLE *h1 = mem_alloc(110, 1);

    mu_assert("test_alloc 111", HW.mem_free == HW.mem + 110);
    mu_assert("test_alloc 112", HW.mem_handle == HW.mem + 200 - sizeof(HANDLE));
    mu_assert("test_alloc 113", HW.mem_stack == HW.mem + 200);
    mu_assert("test_alloc 114", HW.mem_end == HW.mem + 300);

    mu_assert("test_alloc 120", (void *) h1 == (void *) (HW.mem + 200 - sizeof(HANDLE)));
    mu_assert("test_alloc 121", HW.handle_resv_head == h1);
    mu_assert("test_alloc 122", HW.handle_resv_tail == h1);
    mu_assert("test_alloc 123", HW.handle_free_head == 0);

    mu_assert("test_alloc 130", h1->data == (void *) HW.mem);
    mu_assert("test_alloc 131", h1->size == 110);
    mu_assert("test_alloc 132", h1->type == 1);
    mu_assert("test_alloc 133", h1->next == 0);

    HANDLE *h2 = mem_alloc(5, 2);

    mu_assert("test_alloc 211", HW.mem_free == HW.mem + 115);
    mu_assert("test_alloc 212", HW.mem_handle == HW.mem + 200 - 2 * sizeof(HANDLE));
    mu_assert("test_alloc 213", HW.mem_stack == HW.mem + 200);
    mu_assert("test_alloc 214", HW.mem_end == HW.mem + 300);

    mu_assert("test_alloc 2200", (void *) h1 == (void *) (HW.mem + 200 - sizeof(HANDLE)));
    mu_assert("test_alloc 2201", (void *) h2 == (void *) (HW.mem + 200 - 2 * sizeof(HANDLE)));
    mu_assert("test_alloc 221", HW.handle_resv_head == h1);
    mu_assert("test_alloc 222", HW.handle_resv_tail == h2);
    mu_assert("test_alloc 223", HW.handle_free_head == 0);

    mu_assert("test_alloc 230", h1->data == (void *) HW.mem);
    mu_assert("test_alloc 231", h1->size == 110);
    mu_assert("test_alloc 232", h1->type == 1);
    mu_assert("test_alloc 233", h1->next == h2);

    mu_assert("test_alloc 240", h2->data == (void *) (HW.mem + 110));
    mu_assert("test_alloc 241", h2->size == 5);
    mu_assert("test_alloc 242", h2->type == 2);
    mu_assert("test_alloc 243", h2->next == 0);

    call_stack_push(HW.sp);
    call_stack_push(h2);
    call_stack_push(HW.sp);

    HANDLE *h3 = mem_alloc(120, 3);

    mu_assert("test_alloc 311", HW.mem_free == HW.mem + 125);
    mu_assert("test_alloc 312", HW.mem_handle == HW.mem + 200 - 2 * sizeof(HANDLE));
    mu_assert("test_alloc 313", HW.mem_stack == HW.mem + 200);
    mu_assert("test_alloc 314", HW.mem_end == HW.mem + 300);

    mu_assert("test_alloc 3200", (void *) h3 == (void *) (HW.mem + 200 - sizeof(HANDLE)));
    mu_assert("test_alloc 3201", (void *) h2 == (void *) (HW.mem + 200 - 2 * sizeof(HANDLE)));
    mu_assert("test_alloc 321", HW.handle_resv_head == h2);
    mu_assert("test_alloc 322", HW.handle_resv_tail == h3);
    mu_assert("test_alloc 323", HW.handle_free_head == 0);

    mu_assert("test_alloc 330", h3->data == (void *) (HW.mem + 5));
    mu_assert("test_alloc 331", h3->size == 120);
    mu_assert("test_alloc 332", h3->type == 3);
    mu_assert("test_alloc 333", h3->next == 0);

    mu_assert("test_alloc 340", h2->data == (void *) (HW.mem));
    mu_assert("test_alloc 341", h2->size == 5);
    mu_assert("test_alloc 342", h2->type == 2);
    mu_assert("test_alloc 343", h2->next == h3);

    mem_collect_garbage(0);

    mu_assert("test_alloc 411", HW.mem_free == HW.mem + 5);
    mu_assert("test_alloc 412", HW.mem_handle == HW.mem + 200 - 2 * sizeof(HANDLE));
    mu_assert("test_alloc 413", HW.mem_stack == HW.mem + 200);
    mu_assert("test_alloc 414", HW.mem_end == HW.mem + 300);

    mu_assert("test_alloc 4201", (void *) h2 == (void *) (HW.mem + 200 - 2 * sizeof(HANDLE)));
    mu_assert("test_alloc 421", HW.handle_resv_head == h2);
    mu_assert("test_alloc 422", HW.handle_resv_tail == h2);
    mu_assert("test_alloc 423", HW.handle_free_head == ((HANDLE *) (HW.mem + 200 - sizeof(HANDLE))));

    mu_assert("test_alloc 433", ((HANDLE *) (HW.mem + 200 - sizeof(HANDLE)))->next == 0);

    mu_assert("test_alloc 440", h2->data == (void *) (HW.mem));
    mu_assert("test_alloc 441", h2->size == 5);
    mu_assert("test_alloc 442", h2->type == 2);
    mu_assert("test_alloc 443", h2->next == 0);

    call_stack_pop();
    call_stack_pop();
    call_stack_pop();
    mem_collect_garbage(0);

    mu_assert("test_alloc 511", HW.mem_free == HW.mem);
    mu_assert("test_alloc 512", HW.mem_handle == HW.mem + 200 - 2 * sizeof(HANDLE));
    mu_assert("test_alloc 513", HW.mem_stack == HW.mem + 200);
    mu_assert("test_alloc 514", HW.mem_end == HW.mem + 300);

    mu_assert("test_alloc 521", HW.handle_resv_head == 0);
    mu_assert("test_alloc 522", HW.handle_resv_tail == 0);
    mu_assert("test_alloc 523", HW.handle_free_head == ((HANDLE *) (HW.mem + 200 - 2*sizeof(HANDLE))));

    mu_assert("test_alloc 533", ((HANDLE *) (HW.mem + 200 - 2*sizeof(HANDLE)))->next == ((HANDLE *) (HW.mem + 200 - sizeof(HANDLE))));
    mu_assert("test_alloc 534", ((HANDLE *) (HW.mem + 200 - sizeof(HANDLE)))->next == 0);

    HANDLE *h4 = mem_alloc(5, 4);
    HANDLE *h5 = mem_alloc(5, 5);

    mu_assert("test_alloc 611", HW.mem_free == HW.mem+10);
    mu_assert("test_alloc 612", HW.mem_handle == HW.mem + 200 - 2 * sizeof(HANDLE));

    mu_assert("test_alloc 621", HW.handle_resv_head == h4);
    mu_assert("test_alloc 622", HW.handle_resv_tail == h5);
    mu_assert("test_alloc 623", HW.handle_free_head == 0);
    mu_assert("test_alloc 631", h4->next == h5);
    mu_assert("test_alloc 632", h5->next == 0);

    HANDLE *h6 = mem_alloc(5, 6);

    mu_assert("test_alloc 711", HW.mem_free == HW.mem+15);
    mu_assert("test_alloc 712", HW.mem_handle == HW.mem + 200 - 3 * sizeof(HANDLE));

    mu_assert("test_alloc 721", HW.handle_resv_head == h4);
    mu_assert("test_alloc 722", HW.handle_resv_tail == h6);
    mu_assert("test_alloc 723", HW.handle_free_head == 0);
    mu_assert("test_alloc 731", h4->next == h5);
    mu_assert("test_alloc 732", h5->next == h6);
    mu_assert("test_alloc 732", h6->next == 0);

    mem_destroy();
    return 0;
}

static char * test_memory() {
    mu_run_test(test_mem_utilities);
    mu_run_test(test_call_stack);
    mu_run_test(test_alloc);

    return 0;
}
