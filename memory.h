typedef struct handle HANDLE;

struct handle {
	void *data;
	HANDLE *next;
	unsigned int type;
	unsigned int size;
};

struct {
	char *mem;
	void **sp;
} HW;

void init(unsigned int);
void push(void *);
void *pop();
