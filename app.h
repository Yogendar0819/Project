/*
 * Compiler: gcc (GCC) 9.4.0
 * Flags:    -lm -Wall -Wextra -pedantic
 */

#ifndef APP_H
#define APP_H

#include "ll.h"

/* menu action descriptor */
typedef struct {
    const char *desc;
    int        (*func)(ll_t **rinder);
} app_action_t;

/* start the interactive menu */
void app_menu(ll_t **rinder);

#endif /* APP_H */
