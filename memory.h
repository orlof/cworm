//
// Created by Teppo Koskinen on 13/06/15.
//

#ifndef WORM_MEMORY_H
#define WORM_MEMORY_H

// SYSTEM TYPES
#define FLAG_REFERENCED     0x8000

#define TYPE_VOID           0x4000
#define TYPE_CONTROL        0x2000
#define TYPE_EXTENSION      0x1000

// INTERNAL SYSTEM TYPES
#define TYPE_NAME           0x0800
#define TYPE_REFERENCE      0x0400
#define TYPE_SUBSCRIPTION   0x0200
#define TYPE_TUPLE_COMMA    0x0100

//USER TYPES
#define TYPE_NONE           0x0080
#define TYPE_FLOAT          0x0040
#define TYPE_INT            0x0020
#define TYPE_BOOL           0x0010

#define TYPE_STR            0x0008
#define TYPE_TUPLE          0x0004
#define TYPE_LIST           0x0002
#define TYPE_DICT           0x0001

#define TYPE_GROUP_CONTAINER        TYPE_CONTROL | TYPE_TUPLE | TYPE_LIST | TYPE_DICT | TYPE_TUPLE_COMMA | TYPE_REFERENCE | TYPE_SUBSCRIPTION
#define TYPE_GROUP_CAN_REFERENCE    TYPE_LIST | TYPE_DICT | TYPE_STR
#define TYPE_GROUP_INTEGER          TYPE_INT | TYPE_BOOL
#define TYPE_GROUP_NON_INTEGER      !TYPE_GROUP_INTEGER
#define TYPE_GROUP_NUMERIC          TYPE_GROUP_INTEGER | TYPE_FLOAT
#define TYPE_GROUP_ALL              TYPE_SUBSCRIPTION | TYPE_TUPLE_COMMA | TYPE_NONE | TYPE_FLOAT | TYPE_INT | TYPE_BOOL | TYPE_STR | TYPE_TUPLE | TYPEL_LIST | TYPE_DICT

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
