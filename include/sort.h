#ifndef SORT_H
#define SORT_H

#include "list.h"
#include "shape.h"

typedef enum {
    CRIT_DEFAULT,
    CRIT_AREA,
    CRIT_WIDTH,
    CRIT_HEIGHT,
    CRIT_COLOR
} SortCrit;

/* callback chamado apos cada passo significativo do algoritmo */
typedef void (*AnimCallback)(List lst, int k, int frame, void *userdata);

SortCrit sort_parse_crit(const char *s);
int      sort_compare(Shape a, Shape b, SortCrit crit);

void sort_selection(List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);
void sort_bubble   (List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);
void sort_insertion(List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);
void sort_shell    (List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);
void sort_quick    (List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);
void sort_merge    (List lst, int k, SortCrit crit, AnimCallback anim, void *userdata);

#endif