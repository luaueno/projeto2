#ifndef LIST_H
#define LIST_H

#include "shape.h"

/* array dinamico de ponteiros para Shape; nao e dono das shapes */
typedef void *List;

List  list_create(int initial_capacity);
void  list_destroy(List l);
void  list_append(List l, Shape s);
Shape list_get(List l, int i);
void  list_set(List l, int i, Shape s);
void  list_swap(List l, int i, int j);
int   list_size(List l);
void  list_clear(List l);

#endif