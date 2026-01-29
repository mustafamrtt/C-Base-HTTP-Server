#ifndef TPOOL_H
#define TPOOL_H
#include <stdbool.h>
#include <stddef.h>
typedef struct tpool tpool_t;

typedef void(*thread_func_t)(void*);

tpool_t *tpool_create(size_t num);
void tpool_destroy(tpool_t *pool);

bool tpool_add_work(tpool_t *pool, thread_func_t func, void *arg);




#endif