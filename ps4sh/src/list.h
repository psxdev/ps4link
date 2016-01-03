#ifndef _LIST
#define _LIST
struct list {
	char *dir;
	struct list *next;
};
typedef struct list *llist;
#endif
