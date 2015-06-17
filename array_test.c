int counter=0;
void count(HANDLE *h) {
    printf("CALLING\n");
    counter++;
}

static char * test_array_create() {
    mem_initialize(200, 100);
    HANDLE *h1 = array_create(5, TYPE_LIST);

    mu_assert("test_array_create 111", h1->size == sizeof(Array) + 5*sizeof(HANDLE *));
    mu_assert("test_array_create 112", ((Array *) h1->data)->len == 5);

    array_map(count, h1);

    mu_assert("test_array_map 222", counter == 5);

    mem_destroy();
    return 0;
}

static char * test_array() {
    mu_run_test(test_array_create);

    return 0;
}
