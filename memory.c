#include <stdlib.h>
#include "memory.h"

#define AVAILABLE_MEMORY HW.handle_start - HW.heap_free_start

void init(unsigned int size) {
	HW.mem = malloc(size);
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

	if(HW.heap_counter-- == 0) {
		// periodically recycle obsolete handles
		mark();
	}

	// get recycled handle or allocate new
	HANDLE *h = HW.heap_handle_free;
	if(h == 0) {
		h = --HW.heap_handle_start;
	} else {
		HW.heap_handle_free = h->next;
	}

	// initialize HANDLE
	h->ptr = HW.heap_free_start;
	h->size = size;
	h->type = type;
	h->next = 0;

	// remove allocated space from free heap
	HW.heap_free_start += size;

	// append HANDLE to reserved HANDLEs list
	HW.heap_handle_tail->next = h;
	HW.heap_handle_tail = h;

	return h;
}

void mark() {
	for(void **ref=HW.sp, **fp=*HW.sp; ref < MEM_END; ref++) {
		if(ref == fp) {
			fp = *fp;
			continue;
		}
		mark_handle(*ref);
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



