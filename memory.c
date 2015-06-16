#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "array.h"
#include "interpreter.h"

#define HANDLE_COUNTER_LIMIT 100
#define AVAILABLE_MEMORY ((unsigned int) (HW.mem_handle - HW.mem_free))

#define IS_BIT(val, mask) (((val) & (mask)) != 0)
#define SET_BIT(val, bit) ((val) |= (bit))
#define CLR_BIT(val, bit) ((val) &= (~(bit)))

// ============================================================================
// INITIALIZER
// ============================================================================
void mem_initialize(unsigned int heap_size, unsigned int stack_size) {
    #ifdef DEBUG
        printf("mem_initialize\n");
    #endif
	HW.mem = malloc(heap_size + stack_size);

	HW.mem_free         = HW.mem;
    HW.mem_handle       = HW.mem + heap_size;
    HW.mem_stack        = HW.mem + heap_size;
	HW.mem_end          = HW.mem + heap_size + stack_size;

	HW.sp               = (void **) HW.mem_end;

    HW.handle_free_head = 0;
    HW.handle_resv_head = 0;
    HW.handle_resv_tail = 0;
    HW.handle_counter   = HANDLE_COUNTER_LIMIT;
}

void mem_destroy() {
    free(HW.mem);
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

void mem_clear(char *start, unsigned int len) {
    int i;
    for(i=0; i < len; i++) {
        *start++ = 0;
    }
}


// ============================================================================
// CALL STACK
// ============================================================================
void call_stack_push(void *ptr) {
    if((void *) (HW.sp - 1) < (void *) HW.mem_stack) {
        recover("Out of stack space\0");
    }

	*(--HW.sp) = ptr;
}

void *call_stack_pop() {
	return *(HW.sp++);
}

// ============================================================================
// HEAP
// ============================================================================

// ----------------------------------------------------------------------------
// traverse object graph from given root object and set referenced flags
// ----------------------------------------------------------------------------
void mark_handle(HANDLE *handle) {
	if(!IS_BIT(handle->type, FLAG_REFERENCED)) {
		SET_BIT(handle->type, FLAG_REFERENCED);

		if(IS_BIT(handle->type, TYPE_GROUP_CONTAINER)) {
			array_map(&mark_handle, handle);
		}
	}
}

// ----------------------------------------------------------------------------
// traverse all objects from call stack and set referenced flags
// ----------------------------------------------------------------------------
void mark_handles() {
    void **ref;
	for(ref = HW.sp; ref < (void **) HW.mem_end; ref++) {
		if(*ref < (void *) HW.sp) {
			// references always point to memory area below SP
			// all other pointers are skipped as frame pointers
			mark_handle(*ref);
		}
	}
}

// ----------------------------------------------------------------------------
// iterate all handles in reserved handles list and move handles without
// referenced -flag to free handles list
// ----------------------------------------------------------------------------
void rebuild_handle_lists() {
    HANDLE **resv = &HW.handle_resv_head;
    HANDLE **free = &HW.handle_free_head;

    HW.handle_resv_tail = 0;

    HANDLE *handle;
    for(handle=HW.handle_resv_head; handle != 0; handle = handle->next) {
		// which code branch to optimize?
		if(IS_BIT(handle->type, FLAG_REFERENCED)) {
			CLR_BIT(handle->type, FLAG_REFERENCED);
			*resv = handle;
			resv = &(handle->next);
			HW.handle_resv_tail = handle;
		} else {
			*free = handle;
			free = &(handle->next);
		}
	}
    *resv = 0;
    *free = 0;
}

// ----------------------------------------------------------------------------
// allocate handle by recycling free handle or by expanding handle space
// ----------------------------------------------------------------------------
HANDLE *mem_allocate_handle() {
    #ifdef DEBUG
        printf("mem_allocate_handle\n");
    #endif

    if(HW.handle_free_head == 0) {
        HW.mem_handle -= sizeof(HANDLE);
        return (HANDLE *) HW.mem_handle;
    } else {
        HANDLE *handle = HW.handle_free_head;
        HW.handle_free_head = handle->next;
        return handle;
    }
}

// ----------------------------------------------------------------------------
// move handles that call stack does not reference anymore from reserved
// handles list to free handles list
// ----------------------------------------------------------------------------
void mem_unallocate_handles() {
    #ifdef DEBUG
        printf("mem_unallocate_handles\n");
    #endif

	// reset handle counter
	HW.handle_counter = HANDLE_COUNTER_LIMIT;

	mark_handles();
    rebuild_handle_lists();
}

// ----------------------------------------------------------------------------
// iterate all handles in reserved handles list and compact the associated
// data to the start of heap space
//  - handles in reserved list and their data in heap are in same order
// ----------------------------------------------------------------------------
void mem_compact_heap() {
	HW.mem_free = HW.mem;

    HANDLE *handle;
	for(handle=HW.handle_resv_head; handle != 0; handle = handle->next) {
		if(handle->data > (void *) HW.mem_free) {
            // optimize if src==dst
            mem_copy(handle->data, handle->size, HW.mem_free, handle->size);
            handle->data = HW.mem_free;
        }
        HW.mem_free += handle->size;
	}
}

// ----------------------------------------------------------------------------
// full garbage collector
//  - mark objects that are reachable from call stack
//  - free handles that have become unreachable
//  - compact heap
//  - clear free heap
// ----------------------------------------------------------------------------
void mem_collect_garbage(unsigned int required_memory) {
    #ifdef DEBUG
        printf("mem_collect_garbage\n");
    #endif

	mem_unallocate_handles();
	mem_compact_heap();

    if(AVAILABLE_MEMORY < required_memory) {
        recover("Out of memory\0");
    }

	mem_clear(HW.mem_free, AVAILABLE_MEMORY);
}

void mem_realloc(HANDLE *handle, unsigned int size) {
    if(size <= handle->size) {
        handle->size = size;
        return;
    }

    if(AVAILABLE_MEMORY < size) {
        mem_collect_garbage(size);
    }

    mem_copy(handle->data, handle->size, HW.mem_free, size);

    handle->data = HW.mem_free;
    HW.mem_free += size;
}

HANDLE *mem_alloc(unsigned int size, unsigned int type) {
    #ifdef DEBUG
        printf("mem_alloc\n");
    #endif

    if(AVAILABLE_MEMORY < size + sizeof(HANDLE)) {
        mem_collect_garbage((unsigned int) (size + sizeof(HANDLE)));
    }

    if(HW.handle_counter-- == 0) {
        // periodically recycle obsolete handles
        mem_unallocate_handles();
    }

    // get recycled handle or allocate new
    HANDLE *handle = mem_allocate_handle();

    // initialize HANDLE
    handle->data = HW.mem_free;
    handle->size = size;
    handle->type = type;
    handle->next = 0;

    // remove allocated space from free heap
    HW.mem_free += size;

    // append HANDLE to reserved HANDLEs list
    if(HW.handle_resv_head == 0) {
        HW.handle_resv_head = handle;
    } else {
        HW.handle_resv_tail->next = handle;
    }
    HW.handle_resv_tail = handle;

    return handle;
}


