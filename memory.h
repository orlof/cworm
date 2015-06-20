//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_MEMORY_H
#define WORM_MEMORY_H

typedef enum TYPE {
    REFERENCED     = 0x8000;
    VOID           = 0x4000;
    CONTROL        = 0x2000;
    EXTENSION      = 0x1000;

    // INTERNAL SYSTEM TYPES
    NAME           = 0x0800;
    REFERENCE      = 0x0400;
    SUBSCRIPTION   = 0x0200;
    TUPLE_COMMA    = 0x0100;

    //USER TYPES
    NONE           = 0x0080;
    FLOAT          = 0x0040;
    INT            = 0x0020;
    BOOL           = 0x0010;

    STR            = 0x0008;
    TUPLE          = 0x0004;
    LIST           = 0x0002;
    DICT           = 0x0001;
};


// TYPE TESTS

#define IS_TYPE(value, type) ((type & value) != 0)
#define IS_TYPE_CONTAINER(type) ((type & (TYPE.CONTROL | TYPE.TUPLE | TYPE.LIST | TYPE.DICT | TYPE.TUPLE_COMMA | TYPE.REFERENCE | TYPE.SUBSCRIPTION)) != 0)
#define IS_TYPE_REFERENCE(type) ((type & (TYPE.LIST | TYPE.DICT | TYPE.STR)) != 0)
#define IS_TYPE_INTEGER ((type & (TYPE.INT | TYPE.BOOL)) != 0)
#define IS_NOT_TYPE_INTEGER ((type & (TYPE.INT | TYPE.BOOL)) == 0)
#define IS_TYPE_NUMERIC ((type & (TYPE.INT | TYPE.BOOL | TYPE.FLOAT)) != 0)

#define STACK_SIZE     1000
#define HANDLES_SIZE  10000
#define HEAP_SIZE    100000

typedef unsigned int REF;

typedef struct Handle Handle;
struct Handle {
	void *data;
	REF next;
	unsigned int type;
	unsigned int size;
};

struct {
	unsigned int sp;
	REF slot[STACK_SIZE];
} STACK;

struct {
	REF resv_head;
	REF resv_tail;
	REF free_head;

	Handle slot[HANDLES_SIZE];
} HANDLES;

struct {
	char *free;
	char mem[HEAP_SIZE];
} HEAP;

void mem_initialize();
void mem_destroy();

void call_stack_push(REF ref);
REF call_stack_pop();

REF mem_alloc(unsigned int size, unsigned int type);
void mem_realloc(REF ref, unsigned int size);

void mem_copy(char *src, unsigned int src_len, char *dst, unsigned int dst_len);
void mem_clear(char *start, unsigned int len);

void mem_collect_garbage();

#endif //WORM_MEMORY_H
