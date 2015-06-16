#include <stdlib.h>
#include "memory.h"
#include "array.h"
#include "interpreter.h"

#define HANDLE_COUNTER_LIMIT 100
#define AVAILABLE_MEMORY (unsigned int) ((void *) HW.handle_start - (void *) HW.heap_free_start)

#define IS_BIT(val, mask) (((val) & (mask)) != 0)
#define SET_BIT(val, bit) ((val) |= (bit))
#define CLR_BIT(val, bit) ((val) &= (~(bit)))

void init(unsigned int size) {
	HW.mem = malloc(size);

	HW.heap_free_start  = HW.mem;
	HW.mem_end          = HW.mem + size;
	HW.sp               = (void **) (HW.mem + size);

    HW.handle_free_head = 0;
    HW.handle_resv_head = 0;
    HW.handle_resv_tail = 0;
    HW.handle_start     = 0;
    HW.handle_counter   = HANDLE_COUNTER_LIMIT;
}


// ----------------------------------------------------------------------------
// CALL STACK
// ----------------------------------------------------------------------------
void push(void *ptr) {
	*(--HW.sp) = ptr;
}

void *pop() {
	return *(HW.sp++);
}

// ----------------------------------------------------------------------------
// HEAP
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// traverse object graph from given root object and set referenced flags
// ----------------------------------------------------------------------------
void mark_handle(HANDLE *handle) {
	if(IS_BIT(handle->type, FLAG_REFERENCED)) {
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
	for(void **ref=HW.sp; ref < (void **) HW.mem_end; ref++) {
		if(*ref < HW.sp) {
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

    for(HANDLE *h=HW.handle_resv_head; h != 0; h = h->next) {
		// which code branch to optimize?
		if(IS_BIT(h->type, FLAG_REFERENCED)) {
			CLR_BIT(h->type, FLAG_REFERENCED);
			*resv = h;
			resv = &(h->next);
			HW.handle_resv_tail = h;
		} else {
			*free = h;
			free = &(h->next);
		}
	}
    *resv = 0;
    *free = 0;
}

// ----------------------------------------------------------------------------
// move handles that call stack does not reference anymore from reserved
// handles list to free handles list
// ----------------------------------------------------------------------------
void mark() {
	// reset handle counter
	HW.handle_counter = HANDLE_COUNTER_LIMIT;

	mark_handles();
    rebuild_handle_lists();
}

void mem_copy(char *src, unsigned int src_len, char *dst, unsigned int dst_len) {
    for(int i=0; i < dst_len; i++) {
        if(i < src_len) {
            *dst++ = *src++;
        } else {
            *dst++ = 0;
        }
    }
}

void clear(char *start, unsigned int len) {
    for(int i=0; i < len; i++) {
        *start++ = 0;
    }
}

// ----------------------------------------------------------------------------
// iterate all handles in reserved handles list and compact the associated
// data to the start of heap space
//  - handles in reserved list and their data in heap are in same order
// ----------------------------------------------------------------------------
void compact() {
	HW.heap_free_start = HW.mem;

	for(HANDLE *handle=HW.handle_resv_head; handle != 0; handle = handle->next) {
		if(handle->data > HW.heap_free_start) {
            // optimize if src==dst
            mem_copy(handle->data, handle->size, HW.heap_free_start, handle->size);
            handle->data = HW.heap_free_start;
        }
        HW.heap_free_start += handle->size;
	}
}

// ----------------------------------------------------------------------------
// full garbage collector
//  - mark objects that are reachable from call stack
//  - free handles that have become unreachable
//  - compact heap
//  - clear free heap
// ----------------------------------------------------------------------------
void run_garbage_collector(unsigned int required_memory) {
	mark();
	compact();

    if(AVAILABLE_MEMORY < required_memory) {
        recover("Out of memory\0");
    }

	clear(HW.heap_free_start, AVAILABLE_MEMORY);
}

void mem_realloc(HANDLE *handle, unsigned int size) {
    if(size <= handle->size) {
        handle->size = size;
        return;
    }

    if(AVAILABLE_MEMORY < size) {
        run_garbage_collector(size);
    }

    mem_copy(handle->data, handle->size, HW.heap_free_start, size);

    handle->data = HW.heap_free_start;
    HW.heap_free_start += size;
}

HANDLE *get_free_handle() {
    if(HW.handle_free_head == 0) {
        return --HW.handle_free_head;
    } else {
        HANDLE *handle = HW.handle_free_head;
        HW.handle_free_head = handle->next;
        return handle;
    }
}

HANDLE *mem_alloc(unsigned int size, unsigned int type) {
    if(AVAILABLE_MEMORY < size + sizeof(HANDLE)) {
        run_garbage_collector((unsigned int) (size + sizeof(HANDLE)));
    }

    if(HW.handle_counter-- == 0) {
        // periodically recycle obsolete handles
        mark();
    }

    // get recycled handle or allocate new
    HANDLE *handle = get_free_handle();

    // initialize HANDLE
    handle->data = HW.heap_free_start;
    handle->size = size;
    handle->type = type;
    handle->next = 0;

    // remove allocated space from free heap
    HW.heap_free_start += size;

    // append HANDLE to reserved HANDLEs list
    HW.handle_resv_tail->next = handle;
    HW.handle_resv_tail = handle;

    return handle;
}


