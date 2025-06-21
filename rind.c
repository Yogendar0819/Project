#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rind.h"

static int rand_int(int lo, int hi) { return lo + rand() % (hi - lo + 1); }

rind_t *rind_create(const char *name, int age, int milkperday) {
    if (!name || strlen(name)>RIND_NAME_LEN
     || age<0||age>30
     || milkperday<15||milkperday>40)
        return NULL;
    rind_t *r = malloc(sizeof *r);
    if (!r) return NULL;
    memset(r, 0, sizeof *r);
    strncpy(r->name, name, RIND_NAME_LEN);
    r->name[RIND_NAME_LEN] = '\0';
    r->age        = age;
    r->milkperday = milkperday;
    r->status     = RIND_STAT_AVAIL;
    return r;
}

int rind_print(const void *vr) {
    const rind_t *r = vr;
    if (!r) return 0;
    printf("%-15s Alter:%2d Ziel:%2dL/Tag [%s]\n",
           r->name, r->age, r->milkperday,
           r->status==RIND_STAT_AVAIL?"avail":"shed");
    return 1;
}

int rind_print_shed(const void *vr) {
    const rind_t *r = vr;
    if (!r || r->status!=RIND_STAT_COWSHED) return 0;
    rind_print(r);
    if (!r->milk_log) {
        printf("   Keine Milch-Historie vorhanden.\n");
    } else {
        double sum10=0;
        int cnt=0;
        for (ll_t *n=r->milk_log; n; n=n->next) {
            milk_entry_t *e = n->data;
            printf("   %02d.%02d.%04d  %2d.%1dL\n",
                   e->date.day, e->date.month, e->date.year,
                   e->liters/10, e->liters%10);
            sum10 += e->liters;
            ++cnt;
        }
        if (cnt>0) {
            double avg = sum10/10.0/cnt;
            const char *cmp = avg < r->milkperday ? "unter" 
                               : avg > r->milkperday ? "ueber" : "gleich";
            printf("   AVG: %.1fL (%s Ziel %dL)\n",
                   avg, cmp, r->milkperday);
        }
    }
    return 1;
}

int rind_milk(rind_t *r, date_t date) {
    if (!r || r->age<3 || r->status!=RIND_STAT_COWSHED) return 0;
    int amt = rand_int(150,400);
    milk_entry_t *e = malloc(sizeof *e);
    if (!e) return 0;
    e->date = date; e->liters = amt;
    if (!r->milk_log) r->milk_log = ll_new(e);
    else              ll_append(r->milk_log, e);
    return amt;
}

rind_status_t rind_decide(rind_t *r) {
    if (!r) return RIND_STAT_AVAIL;
    if (r->status==RIND_STAT_COWSHED) return RIND_STAT_COWSHED;
    if (rand()%2) r->status = RIND_STAT_COWSHED;
    return r->status;
}

int rind_search_name(const void *vr, const void *vk) {
    const rind_t *r = vr;
    return r && strncmp(r->name, vk, RIND_NAME_LEN+1)==0;
}

void rind_destroy(void *vr) {
    rind_t *r = vr;
    if (!r) return;
    ll_free_all(r->milk_log, free);
    free(r);
}
