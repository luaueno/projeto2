#include "sort.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

SortCrit sort_parse_crit(const char *s) {
    assert(s != NULL);
    if (strcmp(s, "a") == 0) return CRIT_AREA;
    if (strcmp(s, "w") == 0) return CRIT_WIDTH;
    if (strcmp(s, "h") == 0) return CRIT_HEIGHT;
    if (strcmp(s, "c") == 0) return CRIT_COLOR;
    return CRIT_DEFAULT;
}

int sort_compare(Shape a, Shape b, SortCrit crit) {
    assert(a != NULL && b != NULL);
    double va, vb;
    const char *ca, *cb;
    switch (crit) {
        case CRIT_DEFAULT:
            return shape_compare_default(a, b);
        case CRIT_AREA:
            va = shape_get_area(a);
            vb = shape_get_area(b);
            return (va > vb) - (va < vb);
        case CRIT_WIDTH:
            va = shape_get_width(a);
            vb = shape_get_width(b);
            return (va > vb) - (va < vb);
        case CRIT_HEIGHT:
            va = shape_get_height(a);
            vb = shape_get_height(b);
            return (va > vb) - (va < vb);
        case CRIT_COLOR:
            ca = shape_get_fill_color(a);
            cb = shape_get_fill_color(b);
            return strcmp(ca, cb);
        default:
            assert(0 && "criterio desconhecido");
    }
    return 0;
}

static void emit(List lst, int k, int *frame, AnimCallback anim, void *ud) {
    if (anim) {
        anim(lst, k, *frame, ud);
        (*frame)++;
    }
}

void sort_selection(List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    int n = list_size(lst);
    int frame = 1;
    int i, j;
    for (i = 0; i < k && i < n; i++) {
        int min_idx = i;
        for (j = i + 1; j < n; j++)
            if (sort_compare(list_get(lst, j), list_get(lst, min_idx), crit) < 0)
                min_idx = j;
        if (min_idx != i) list_swap(lst, i, min_idx);
        emit(lst, k, &frame, anim, userdata);
    }
}

void sort_bubble(List lst, int k, SortCrit crit,
                 AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    int n = list_size(lst);
    int frame = 1;
    int i, j;
    for (i = 0; i < k && i < n - 1; i++) {
        for (j = n - 1; j > i; j--) {
            if (sort_compare(list_get(lst, j), list_get(lst, j - 1), crit) < 0) {
                list_swap(lst, j, j - 1);
                emit(lst, k, &frame, anim, userdata);
            }
        }
    }
}

void sort_insertion(List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    (void)k;
    int n = list_size(lst);
    int frame = 1;
    int i, j;
    for (i = 1; i < n; i++) {
        j = i;
        while (j > 0 && sort_compare(list_get(lst, j), list_get(lst, j - 1), crit) < 0) {
            list_swap(lst, j, j - 1);
            j--;
        }
        emit(lst, k, &frame, anim, userdata);
    }
}

void sort_shell(List lst, int k, SortCrit crit,
                AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    int n = list_size(lst);
    int frame = 1;
    int gap, i, j;
    for (gap = n / 2; gap > 0; gap /= 2) {
        for (i = gap; i < n; i++) {
            j = i;
            while (j >= gap &&
                   sort_compare(list_get(lst, j), list_get(lst, j - gap), crit) < 0) {
                list_swap(lst, j, j - gap);
                j -= gap;
            }
        }
        emit(lst, k, &frame, anim, userdata);
    }
}

static int partition(List lst, int lo, int hi, SortCrit crit) {
    Shape pivot = list_get(lst, hi);
    int i = lo - 1, j;
    for (j = lo; j < hi; j++) {
        if (sort_compare(list_get(lst, j), pivot, crit) <= 0) {
            i++;
            list_swap(lst, i, j);
        }
    }
    list_swap(lst, i + 1, hi);
    return i + 1;
}

static void quick_rec(List lst, int lo, int hi, int k, SortCrit crit,
                       int *frame, AnimCallback anim, void *ud) {
    if (lo >= hi) return;
    int p = partition(lst, lo, hi, crit);
    emit(lst, k, frame, anim, ud);
    if (p > lo) quick_rec(lst, lo, p - 1, k, crit, frame, anim, ud);
    if (p < k)  quick_rec(lst, p + 1, hi, k, crit, frame, anim, ud);
}

void sort_quick(List lst, int k, SortCrit crit,
                AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    int n = list_size(lst);
    int frame = 1;
    if (n > 1)
        quick_rec(lst, 0, n - 1, k, crit, &frame, anim, userdata);
}

static void merge(List lst, int lo, int mid, int hi, SortCrit crit,
                  int *frame, int k, AnimCallback anim, void *ud) {
    int left  = mid - lo + 1;
    int right = hi  - mid;
    int i, j, idx;

    Shape *L = (Shape *)malloc(sizeof(Shape) * (size_t)left);
    Shape *R = (Shape *)malloc(sizeof(Shape) * (size_t)right);
    assert(L && R);

    for (i = 0; i < left;  i++) L[i] = list_get(lst, lo  + i);
    for (j = 0; j < right; j++) R[j] = list_get(lst, mid + 1 + j);

    i = 0; j = 0; idx = lo;
    while (i < left && j < right) {
        if (sort_compare(L[i], R[j], crit) <= 0)
            list_set(lst, idx++, L[i++]);
        else
            list_set(lst, idx++, R[j++]);
    }
    while (i < left)  list_set(lst, idx++, L[i++]);
    while (j < right) list_set(lst, idx++, R[j++]);

    free(L); free(R);
    emit(lst, k, frame, anim, ud);
}

static void merge_rec(List lst, int lo, int hi, SortCrit crit,
                      int *frame, int k, AnimCallback anim, void *ud) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    merge_rec(lst, lo,      mid, crit, frame, k, anim, ud);
    merge_rec(lst, mid + 1, hi,  crit, frame, k, anim, ud);
    merge(lst, lo, mid, hi, crit, frame, k, anim, ud);
}

void sort_merge(List lst, int k, SortCrit crit,
                AnimCallback anim, void *userdata) {
    assert(lst != NULL);
    int n = list_size(lst);
    int frame = 1;
    if (n > 1)
        merge_rec(lst, 0, n - 1, crit, &frame, k, anim, userdata);
}