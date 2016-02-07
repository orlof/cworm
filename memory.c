#include <stdio.h>
#include "memory.h"
#include "array.h"
#include "interpreter.h"

#define AVAILABLE_MEMORY (HEAP.mem + HEAP_SIZE - HEAP.free)

#define IS_BIT(val, mask) (((val) & (mask)) != 0)
#define SET_BIT(val, bit) ((val) |= (bit))
#define CLR_BIT(val, bit) ((val) &= (~(bit)))

// ============================================================================
// INITIALIZER
// ============================================================================
void mem_initialize() {
    #ifdef DEBUG
        printf("mem_initialize\n");
    #endif

    // init handles
    REF i;
    for(i = 0; i < HANDLES_SIZE; i++) {
        HANDLES.slot[i].next = i + 1;
    }
    HANDLES.resv_head = HANDLES_SIZE;
    HANDLES.resv_tail = HANDLES_SIZE;
    HANDLES.free_head = 0;

    STACK.sp = STACK_SIZE;
    HEAP.free = HEAP.mem;
}

void mem_destroy() {
}

// ============================================================================
// GENERIC UTILITIES
// ============================================================================
void mem_copy(char *src, unsigned int src_len, char *dst, unsigned int dst_len) {
    int i;
    for(i=0; i < dst_len; i++) {
        if(i < src_len) {
            *dst++ = *src++;
        } else {
            *dst++ = 0;
        }
    }
}

void mem_clear(char *start, char *end) {
    for(; start < end; start++) {
        *start = 0;
    }
}


// ============================================================================
// CALL STACK
// ============================================================================
void call_stack_push(REF ref) {
    if(STACK.sp == 0) {
        recover("Out of stack space\0");
    }

	STACK.slot[--STACK.sp] = ref;
}

REF call_stack_pop() {
	return STACK.slot[STACK.sp++];
}

// ============================================================================
// HEAP
// ============================================================================

// ----------------------------------------------------------------------------
// traverse object graph from given root object and set referenced flags
// ----------------------------------------------------------------------------
void mem_mark_handle(REF ref) {
    Handle *handle = &HANDLES.slot[ref];
    
    if(!IS_BIT(handle->flags, REFERENCED)) {
		SET_BIT(handle->flags, REFERENCED);

		if(IS_BIT(handle->flags, TYPE_CONTAINERS)) {
			array_map(ref, &mem_mark_handle);
		}
	}
}

// ----------------------------------------------------------------------------
// traverse all objects from call stack and set referenced flags
// ----------------------------------------------------------------------------
void mem_mark() {
    unsigned int i;
    for(i = STACK.sp; i < STACK_SIZE; i++) {
        mem_mark_handle(STACK.slot[i]);
    }
}

// ----------------------------------------------------------------------------
// iterate all handles in reserved handles list and move handles without
// referenced -flag to free handles list
// ----------------------------------------------------------------------------
void mem_compact() {
    #ifdef DEBUG
        printf("mem_compact\n");
    #endif

    REF *resv = &HANDLES.resv_head;
    HANDLES.resv_tail = HANDLES_SIZE;

    HEAP.free = HEAP.mem;
 
    REF ref, next;
    for(ref = HANDLES.resv_head; ref != HANDLES_SIZE; ref = next) {
        Handle *handle = &HANDLES.slot[ref];
        next = handle->next;

		// which code branch to optimize?
		if(IS_BIT(handle->flags, REFERENCED)) {
			CLR_BIT(handle->flags, REFERENCED);
			*resv = ref;
			resv = &handle->next;
			HANDLES.resv_tail = ref;

            if(handle->data != HEAP.free) {
                mem_copy(handle->data, handle->size, HEAP.free, handle->size);
                handle->data = HEAP.free;
            }
            HEAP.free += handle->size;
		} else {
            handle->next = HANDLES.free_head;
            HANDLES.free_head = ref;
		}
	}
    *resv = HANDLES_SIZE;
}

// ----------------------------------------------------------------------------
// allocate handle by recycling free handle or by expanding handle space
// ----------------------------------------------------------------------------
REF mem_allocate_handle() {
    #ifdef DEBUG
        printf("mem_allocate_handle\n");
    #endif

    if(HANDLES.free_head == HANDLES_SIZE) {
        recover("Out of handle space error\0");
    }

    REF ref = HANDLES.free_head;
    HANDLES.free_head = HANDLES.slot[ref].next;
    return ref;
}

// ----------------------------------------------------------------------------
// full garbage collector
//  - mark objects that are reachable from call stack
//  - free handles that have become unreachable
//  - compact heap
//  - clear free heap
// ----------------------------------------------------------------------------
void mem_collect_garbage() {
    #ifdef DEBUG
        printf("mem_collect_garbage\n");
    #endif

    mem_mark();
    mem_compact();

	mem_clear(HEAP.free, HEAP.mem + HEAP_SIZE);
}

void mem_realloc(REF ref, unsigned int size) {
    Handle *handle = &HANDLES.slot[ref];
    if(size <= handle->size) {
        handle->size = size;
        return;
    }

    if(AVAILABLE_MEMORY < size) {
        mem_collect_garbage();
        if(AVAILABLE_MEMORY < size) {
            recover("Out of memory error\0");
        }
    }

    mem_copy(handle->data, handle->size, HEAP.free, size);

    handle->size = size;
    handle->data = HEAP.free;
    HEAP.free += size;
}

void debug_list(REF ref) {
    while(ref != HANDLES_SIZE) {
        Handle *handle = &HANDLES.slot[ref];
        printf("  [%d] {%d, %d, %d}\n", ref, handle->size, handle->flags, handle->next);
        ref = handle->next;
    }
}

REF mem_alloc(unsigned int size, FLAGS type) {
    #ifdef DEBUG
        printf("mem_alloc\n");
    #endif

    if(AVAILABLE_MEMORY < size) {
        mem_collect_garbage();
        if(AVAILABLE_MEMORY < size) {
            recover("Out of memory error\0");
        }
    }

    // get recycled handle or allocate new
    REF ref = mem_allocate_handle();

    // initialize HANDLE
    Handle *handle = &HANDLES.slot[ref];
    handle->data = HEAP.free;
    handle->size = size;
    handle->flags = type;
    handle->next = HANDLES_SIZE;

    // remove allocated space from free heap
    HEAP.free += size;

    // append HANDLE to reserved HANDLEs list
    if(HANDLES.resv_head == HANDLES_SIZE) {
        HANDLES.resv_head = ref;
    } else {
        Handle *tail = &HANDLES.slot[HANDLES.resv_tail];
        tail->next = ref;
    }
    HANDLES.resv_tail = ref;

    return ref;
}

