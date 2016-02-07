//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_MEMORY_H
#define WORM_MEMORY_H

typedef enum {
    REFERENCED     = 0x8000,
    VOID           = 0x4000,
    CONTROL        = 0x2000,
    EXTENSION      = 0x1000,

    // INTERNAL SYSTEM TYPES
    NAME           = 0x0800,
    REFERENCE      = 0x0400,
    SUBSCRIPTION   = 0x0200,
    TUPLE_COMMA    = 0x0100,

    //USER TYPES
    NONE           = 0x0080,
    FLOAT          = 0x0040,
    INT            = 0x0020,
    BOOL           = 0x0010,

    STR            = 0x0008,
    TUPLE          = 0x0004,
    LIST           = 0x0002,
    DICT           = 0x0001,
} FLAGS;


// TYPE TESTS

#define TYPE_CONTAINERS (CONTROL | TUPLE | LIST | DICT | TUPLE_COMMA | REFERENCE | SUBSCRIPTION)

#define IS_TYPE_REFERENCE(type) ((type & (LIST | DICT | STR)) != 0)

#define IS_TYPE_INTEGER(type) ((type & (INT | BOOL)) != 0)
#define IS_TYPE_FLOAT(type) ((type & (FLOAT)) != 0)
#define IS_TYPE_LITERAL(type) ((type & (STRING | NAME)) != 0)
#define IS_TYPE_ARRAY(type) ((type & (TUPLE | LIST | DICT)) != 0)

#define IS_NOT_TYPE_INTEGER ((type & (INT | BOOL)) == 0)
#define IS_TYPE_NUMERIC ((type & (INT | BOOL | FLOAT)) != 0)

#define STACK_SIZE     1000
#define HANDLES_SIZE  10000
#define HEAP_SIZE    100000

#define ILLEGAL_REF ((REF) HANDLES_SIZE)

typedef unsigned int REF;

typedef struct Handle Handle;
struct Handle {
	void *data;
	REF next;
	FLAGS flags;
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

REF mem_alloc(unsigned int size, FLAGS type);
void mem_realloc(REF ref, unsigned int size);

void mem_copy(char *src, unsigned int src_len, char *dst, unsigned int dst_len);
void mem_clear(char *start, char *end);

void mem_collect_garbage();

#endif //WORM_MEMORY_H
