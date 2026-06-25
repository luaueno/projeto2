#include "list.h"
#include <stdlib.h>
#include <assert.h>

typedef struct {
    Shape *items;
    int    size;
    int    capacity;
} ListData;

List list_create(int initial_capacity) {
    if (initial_capacity < 1) initial_capacity = 4;
    ListData *ld = (ListData *)malloc(sizeof(ListData));
    if (!ld) return NULL;
    ld->items = (Shape *)malloc(sizeof(Shape) * (size_t)initial_capacity);
    if (!ld->items) { free(ld); return NULL; }
    ld->size     = 0;
    ld->capacity = initial_capacity;
    return (List)ld;
}

void list_destroy(List l) {
    assert(l != NULL);
    ListData *ld = (ListData *)l;
    free(ld->items);
    free(ld);
}

void list_append(List l, Shape s) {
    assert(l != NULL);
    ListData *ld = (ListData *)l;
    if (ld->size == ld->capacity) {
        int new_cap = ld->capacity * 2;
        Shape *tmp = (Shape *)realloc(ld->items, sizeof(Shape) * (size_t)new_cap);
        assert(tmp != NULL);
        ld->items    = tmp;
        ld->capacity = new_cap;
    }
    ld->items[ld->size++] = s;
}

Shape list_get(List l, int i) {
    assert(l != NULL);
    ListData *ld = (ListData *)l;
    assert(i >= 0 && i < ld->size);
    return ld->items[i];
}

void list_set(List l, int i, Shape s) {
    assert(l != NULL);
    ListData *ld = (ListData *)l;
    assert(i >= 0 && i < ld->size);
    ld->items[i] = s;
}

void list_swap(List l, int i, int j) {
    assert(l != NULL);
    ListData *ld = (ListData *)l;
    assert(i >= 0 && i < ld->size);
    assert(j >= 0 && j < ld->size);
    Shape tmp    = ld->items[i];
    ld->items[i] = ld->items[j];
    ld->items[j] = tmp;
}

int list_size(List l) {
    assert(l != NULL);
    return ((ListData *)l)->size;
}

void list_clear(List l) {
    assert(l != NULL);
    ((ListData *)l)->size = 0;
}