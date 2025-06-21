/*
 * Compiler: gcc (GCC) 9.4.0
 * Flags:    -lm -Wall -Wextra -pedantic
 */

#ifndef RIND_H
#define RIND_H

#include "ll.h"

#define RIND_NAME_LEN 30

typedef enum { RIND_STAT_AVAIL, RIND_STAT_COWSHED } rind_status_t;
typedef struct { int day, month, year; } date_t;
typedef struct {
    date_t date;
    int    liters;  /* milk ×10 */
} milk_entry_t;

typedef struct {
    char           name[RIND_NAME_LEN+1];
    int            age;
    int            milkperday;
    rind_status_t  status;
    ll_t          *milk_log;
} rind_t;

/* 0.4.1 */
rind_t        *rind_create(const char *name, int age, int milkperday);
/* 0.4.2 */
int            rind_print(const void *r);
/* 0.4.3 */
int            rind_print_shed(const void *r);
/* 0.4.4 */
int            rind_milk(rind_t *r, date_t date);
/* 0.4.5 */
rind_status_t  rind_decide(rind_t *r);
/* 0.4.6 */
int            rind_search_name(const void *r, const void *key);
/* cleanup */
void           rind_destroy(void *r);

#endif /* RIND_H */
