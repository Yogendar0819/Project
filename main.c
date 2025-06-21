/*
 * Compiler: gcc (GCC) 9.4.0
 * Flags:    -lm -Wall -Wextra -pedantic
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "ll.h"
#include "rind.h"
#include "app.h"

/* 0.2 Matrikelnummer */
int matrikelnr(char *nr, int maxlen) {
    const char *id = "5434132";  /* <-- your own */
    if ((int)strlen(id) < maxlen) {
        strcpy(nr,id); return 0;
    }
    return -1;
}

int main(void) {
    srand((unsigned)time(NULL));
    ll_t *herd = NULL;
    app_menu(&herd);
    ll_free_all(herd, rind_destroy);
    return 0;
}
