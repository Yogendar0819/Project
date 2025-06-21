#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "app.h"
#include "rind.h"
#include "ll.h"

static const char FILENAME[] = "rinder.bin";

/* clear stdin until newline */
static void flush_stdin(void) {
    int c; while ((c=getchar())!='\n' && c!=EOF);
}

/* read a line and strip newline */
static int read_line(char *buf, int len) {
    if (!fgets(buf, len, stdin)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

/* parse int safely */
static int read_int(const char *prompt, int *out, int min, int max) {
    char buf[32];
    while (1) {
        printf("%s", prompt);
        if (!read_line(buf, sizeof buf)) return 0;
        char *end; long v = strtol(buf, &end, 10);
        if (*end!='\0' || v<min || v>max) {
            printf("Ungueltige Eingabe!\n");
            continue;
        }
        *out = (int)v;
        return 1;
    }
}

/* 1) create */
static int act_create_rind(ll_t **pr) {
    char name[64];
    printf("Name (max %d): ", RIND_NAME_LEN);
    if (!read_line(name, sizeof name)) return 1;
    if (!*name || strlen(name)>RIND_NAME_LEN) {
        puts("Ungueltiger Name!"); return 1;
    }
    if (ll_search(*pr, rind_search_name, name, NULL)) {
        puts("Name existiert bereits."); return 1;
    }
    int age, perf;
    if (!read_int("Alter (0-30): ", &age, 0, 30)) return 1;
    if (!read_int("Milch/Tag (15-40): ", &perf, 15, 40)) return 1;
    rind_t *r = rind_create(name, age, perf);
    if (!r) { puts("Fehler beim Anlegen."); return 1; }
    if (!*pr) *pr = ll_new(r); else ll_append(*pr, r);
    printf("Rind '%s' angelegt.\n", r->name);
    return 1;
}

/* 2) swipe */
static int act_swipe_rinder(ll_t **pr) {
    if (!*pr) { puts("Keine Rinder."); return 1; }
    int any = 0;
    for (ll_t *n = *pr; n; n=n->next) {
        rind_t *r = n->data;
        if (r->status==RIND_STAT_AVAIL) {
            any = 1; rind_print(r);
            printf("In Stall aufnehmen? (j/n): ");
            char ans = getchar(); flush_stdin();
            if (ans=='j' || ans=='J') {
                rind_status_t st = rind_decide(r);
                printf("  => Rind '%s' %s Stall.\n",
                       r->name,
                       st==RIND_STAT_COWSHED? "kommt in den":"bleibt im");
            }
        }
    }
    if (!any) puts("Keine verfuegbaren Rinder.");
    return 1;
}

/* 3) print shed */
static int act_print_shed(ll_t **pr) {
    puts("Rinder im Stall:");
    if (!*pr || !ll_print(*pr, rind_print_shed))
        puts("  (keine)");
    return 1;
}

/* 4) milk */
static int act_milk_rinder(ll_t **pr) {
    if (!*pr) { puts("Keine Rinder."); return 1; }
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    date_t d = {tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900};
    int total = 0, any = 0;
    for (ll_t *n=*pr; n; n=n->next) {
        rind_t *r = n->data;
        if (r->status==RIND_STAT_COWSHED) {
            int amt = rind_milk(r, d);
            if (amt) { any = 1; total += amt; }
        }
    }
    if (!any) puts("Keines der Rinder hat Milch gegeben.");
    else      printf("Total: %.1f L\n", total/10.0);
    return 1;
}

/* 5) delete */
static int act_delete_rind(ll_t **pr) {
    if (!*pr) { puts("Keine Rinder."); return 1; }
    char name[64];
    printf("Name loeschen: ");
    if (!read_line(name, sizeof name)) return 1;
    int idx; ll_t *f = ll_search(*pr, rind_search_name, name, &idx);
    if (!f) { puts("Nicht gefunden."); return 1; }
    rind_destroy(f->data);
    ll_remove(pr, idx);
    printf("Rind '%s' geloescht.\n", name);
    return 1;
}

/* 6) save */
static int act_save_rinder(ll_t **pr) {
    FILE *f = fopen(FILENAME,"wb");
    if (!f) { perror("save"); return 1; }
    size_t n = ll_count(*pr);
    fwrite(&n, sizeof n, 1, f);
    for (ll_t *c=*pr; c; c=c->next) {
        rind_t *r = c->data;
        fwrite(r->name, sizeof r->name, 1, f);
        fwrite(&r->age, sizeof r->age, 1, f);
        fwrite(&r->milkperday, sizeof r->milkperday, 1, f);
        fwrite(&r->status, sizeof r->status, 1, f);
        size_t m = ll_count(r->milk_log);
        fwrite(&m, sizeof m, 1, f);
        for (ll_t *e=r->milk_log; e; e=e->next) {
            milk_entry_t *me = e->data;
            fwrite(&me->date, sizeof me->date, 1, f);
            fwrite(&me->liters, sizeof me->liters, 1, f);
        }
    }
    fclose(f);
    printf("Rinder gespeichert (%lu).\n", (unsigned long)n);
    return 1;
}

/* 7) load */
static int act_load_rinder(ll_t **pr) {
    FILE *f = fopen(FILENAME,"rb");
    if (!f) { perror("load"); return 1; }
    ll_free_all(*pr, rind_destroy);
    *pr = NULL;
    size_t n; fread(&n, sizeof n, 1, f);
    for (size_t i=0; i<n; i++) {
        rind_t *r = calloc(1, sizeof *r);
        fread(r->name, sizeof r->name, 1, f);
        fread(&r->age, sizeof r->age, 1, f);
        fread(&r->milkperday, sizeof r->milkperday, 1, f);
        fread(&r->status, sizeof r->status, 1, f);
        size_t m; fread(&m, sizeof m, 1, f);
        for (size_t j=0; j<m; j++) {
            milk_entry_t *me = malloc(sizeof *me);
            fread(&me->date, sizeof me->date, 1, f);
            fread(&me->liters, sizeof me->liters, 1, f);
            if (!r->milk_log) r->milk_log = ll_new(me);
            else              ll_append(r->milk_log, me);
        }
        if (!*pr) *pr = ll_new(r);
        else      ll_append(*pr, r);
    }
    fclose(f);
    printf("Rinder geladen (%lu).\n", (unsigned long)n);
    return 1;
}

/* 8) exit */
static int act_exit_program(ll_t **pr) {
    (void)pr;
    puts("Programm beendet.");
    return 0;
}

/* 0.6 menu */
void app_menu(ll_t **r) {
    static const app_action_t A[] = {
        { "Rind anlegen",      act_create_rind },
        { "Rinder swipen",     act_swipe_rinder },
        { "Stall anzeigen",    act_print_shed },
        { "Rinder melken",     act_milk_rinder },
        { "Rind loeschen",     act_delete_rind },
        { "Speichern",         act_save_rinder },
        { "Laden",             act_load_rinder },
        { "Beenden",           act_exit_program }
    };
    const int N = sizeof A / sizeof *A;

    while (1) {
        printf("\n--- Menue ---\n");
        for (int i=0; i<N; i++)
            printf("%d) %s\n", i+1, A[i].desc);
        printf("Auswahl (1-%d): ", N);

        char buf[16];
        if (!fgets(buf, sizeof buf, stdin)) break;
        int c = strtol(buf, NULL, 10);
        if (c<1 || c>N) { puts("Ungueltig!"); continue; }
        if (!A[c-1].func(r)) break;
    }
}
