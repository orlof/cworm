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

typedef struct handle HANDLE;

struct handle {
	void *data;
	HANDLE *next;
	unsigned int type;
	unsigned int size;
};

struct {
	void **sp;
	char *mem;
	char *mem_free;
	char *mem_handle;
	char *mem_stack;
	char *mem_end;
	HANDLE *handle_free_head;
	HANDLE *handle_resv_head;
	HANDLE *handle_resv_tail;
	unsigned int handle_counter;
} HW;

void mem_initialize(unsigned int heap_size, unsigned int stack_size);
void mem_destroy();

void call_stack_push(void *ptr);
void *call_stack_pop();

HANDLE * mem_alloc(unsigned int size, unsigned int type);
void mem_realloc(HANDLE *handle, unsigned int size);

void mem_copy(char *src, unsigned int src_len, char *dst, unsigned int dst_len);
void mem_clear(char *start, unsigned int len);

void mem_collect_garbage(unsigned int required_memory);

#endif //WORM_MEMORY_H
