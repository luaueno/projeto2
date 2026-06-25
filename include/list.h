#ifndef LIST_H
#define LIST_H

#include "shape.h"

/*
 * list.h - Interface publica da lista dinamica de formas geometricas.
 *
 * List e um ponteiro opaco para um array dinamico de Shape. A lista NAO
 * e dona das shapes: ela armazena apenas ponteiros. Quem criou as shapes
 * e responsavel por destrui-las.
 *
 * Crescimento: dobra a capacidade quando necessario (realloc).
 */

typedef void *List;

/* Cria lista com capacidade inicial. Retorna NULL em falha. */
List list_create(int initial_capacity);

/* Libera a estrutura da lista (nao libera as shapes). */
void list_destroy(List l);

/* Acrescenta shape ao final. Cresce automaticamente se necessario. */
void list_append(List l, Shape s);

/* Retorna shape no indice i (sem checagem de bounds em release). */
Shape list_get(List l, int i);

/* Substitui o shape no indice i. */
void list_set(List l, int i, Shape s);

/* Troca os shapes nos indices i e j. */
void list_swap(List l, int i, int j);

/* Retorna o numero de elementos. */
int list_size(List l);

/* Esvazia a lista sem liberar as shapes. */
void list_clear(List l);

#endif /* LIST_H */