typedef struct handle HANDLE;

struct handle {
	void *ptr;
	HANDLE *next;
	unsigned int type;
	unsigned int size;
};

struct {
	void **sp;
	char *mem;
	char *mem_end;
	char *heap_free_start;
	HANDLE *handle_free_head;
	HANDLE *handle_reserved_head;
	HANDLE *handle_reserved_tail;
	HANDLE *handle_start;
	unsigned int handle_counter;
} HW;

void init(unsigned int);
void push(void *);
void *pop();
