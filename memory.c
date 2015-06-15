#include <stdlib.h>
#include "memory.h"

#define HANDLE_COUNTER_LIMIT 100
#define AVAILABLE_MEMORY HW.handle_start - HW.heap_free_start

void init(unsigned int size) {
	HW.mem = malloc(size);
	HW.mem_end = HW.mem + size;
	HW.sp = (void **) (HW.mem + size);
}

void push(void *ptr) {
	*(--HW.sp) = ptr;
}

void *pop() {
	return *(HW.sp++);
}

HANDLE *alloc(unsigned int size, unsigned int type) {
	if(AVAILABLE_MEMORY < size + sizeof(HANDLE)) {
		free_memory();
	}

	if(HW.handle_counter-- == 0) {
		// periodically recycle obsolete handles
		mark();
	}

	// get recycled handle or allocate new
	HANDLE *h = HW.handle_free_head;
	if(h == 0) {
		h = --HW.handle_start;
	} else {
		HW.handle_free_head = h->next;
	}

	// initialize HANDLE
	h->ptr = HW.heap_free_start;
	h->size = size;
	h->type = type;
	h->next = 0;

	// remove allocated space from free heap
	HW.heap_free_start += size;

	// append HANDLE to reserved HANDLEs list
	HW.handle_reserved_tail->next = h;
	HW.handle_reserved_tail = h;

	return h;
}

void mark() {
	HW.handle_counter = HANDLE_COUNTER_LIMIT;

	for(void **ref=HW.sp, **fp=*HW.sp; ref < HW.mem_end; ref++) {
		if(ref == fp) {
			fp = *fp;
			continue;
		}
		mark_handle(*ref);
	}

	for(HANDLE *h=HW.handle_reserved_head; h != 0; h = h->next) {
		if(h->type & TYPE_MARKED > 0) {
			// clear mark
			h->type ^= TYPE_MARKED;

		} else {

		}
	}
}

void mark_handle(HANDLE *h) {
	if(h->type & TYPE_MARKED == 0) {
		h->type |= TYPE_MARKED;

		if(h->type & TYPE_GROUP_CONTAINER != 0) {
			array_map(mark_handle, h);
		}
	}
}



