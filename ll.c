#include <stdlib.h>
#include "ll.h"

ll_t *ll_new(void *data) {
    ll_t *n = malloc(sizeof *n);
    if (!n) return NULL;
    n->data = data;
    n->next = NULL;
    return n;
}

void ll_append(ll_t *ll, void *data) {
    if (!ll) return;
    while (ll->next) ll = ll->next;
    ll->next = ll_new(data);
}

ll_t *ll_get(ll_t *ll, size_t idx) {
    while (ll && idx--) ll = ll->next;
    return ll;
}

void ll_remove(ll_t **p_ll, size_t index) {
    if (!p_ll || !*p_ll) return;
    ll_t *cur = *p_ll, *prev = NULL;
    while (cur && index--) {
        prev = cur;
        cur  = cur->next;
    }
    if (!cur) return;
    if (prev) prev->next = cur->next;
    else       *p_ll     = cur->next;
    free(cur);
}

size_t ll_count(const ll_t *ll) {
    size_t c = 0;
    while (ll) { ++c; ll = ll->next; }
    return c;
}

ll_t *ll_search(ll_t *ll,
                int (*search_fn)(const void *, const void *),
                const void *key,
                int *idx) {
    int i = 0;
    while (ll) {
        if (search_fn(ll->data, key)) {
            if (idx) *idx = i;
            return ll;
        }
        ll = ll->next; ++i;
    }
    if (idx) *idx = -1;
    return NULL;
}

int ll_print(const ll_t *ll, int (*print_fn)(const void *)) {
    int printed = 0;
    while (ll) {
        if (print_fn && print_fn(ll->data)) ++printed;
        ll = ll->next;
    }
    return printed;
}

void ll_free_all(ll_t *ll, void (*free_fn)(void *)) {
    while (ll) {
        ll_t *n = ll->next;
        if (free_fn) free_fn(ll->data);
        free(ll);
        ll = n;
    }
}
