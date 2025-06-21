/*
 * Compiler: gcc (GCC) 9.4.0
 * Flags:    -lm -Wall -Wextra -pedantic
 */

#ifndef LL_H
#define LL_H

#include <stddef.h>

/* Node of a generic singly-linked list */
typedef struct ll_s {
    void        *data;
    struct ll_s *next;
} ll_t;

/* 0.3.1 */
ll_t   *ll_new(void *data);

/* 0.3.2 */
void     ll_append(ll_t *ll, void *data);

/* 0.3.3 */
ll_t   *ll_get(ll_t *ll, size_t index);

/* 0.3.4 */
void     ll_remove(ll_t **p_ll, size_t index);

/* 0.3.5 */
size_t   ll_count(const ll_t *ll);

/* 0.3.6 */
ll_t   *ll_search(ll_t *ll,
                  int (*search_fn)(const void *, const void *),
                  const void *key,
                  int *idx);

/* 0.3.7 */
int      ll_print(const ll_t *ll, int (*print_fn)(const void *));

/* cleanup */
void     ll_free_all(ll_t *ll, void (*free_fn)(void *));

#endif /* LL_H */

