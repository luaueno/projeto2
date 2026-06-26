#ifndef SORT_H
#define SORT_H

#include "list.h"
#include "shape.h"

/*
 * sort.h - Interface publica dos algoritmos de ordenacao com animacao.
 *
 * Cada algoritmo recebe uma List de shapes, o numero k de menores
 * elementos desejados, um criterio de comparacao e um callback de
 * animacao. O callback e chamado a cada passo relevante do algoritmo,
 * permitindo gerar frames SVG da animacao.
 *
 * Apos a execucao, os primeiros k elementos da lista sao os k menores
 * segundo o criterio escolhido (em ordem crescente).
 */

/* Criterios de ordenacao. */
typedef enum {
    CRIT_DEFAULT,  /* d: ordem default (y, x, area) */
    CRIT_AREA,     /* a: por area                   */
    CRIT_WIDTH,    /* w: por largura                */
    CRIT_HEIGHT,   /* h: por altura                 */
    CRIT_COLOR     /* c: por cor de preenchimento   */
} SortCrit;

/*
 * Callback de animacao chamado apos cada passo significativo.
 *   lst      : estado atual da lista em ordenacao
 *   k        : quantidade de menores elementos buscados
 *   frame    : numero sequencial do frame (comeca em 1)
 *   userdata : ponteiro opaco passado de volta ao chamador
 */
typedef void (*AnimCallback)(List lst, int k, int frame, void *userdata);

/* Converte string ("d","a","w","h","c") para SortCrit. */
SortCrit sort_parse_crit(const char *s);

/*
 * Compara duas shapes segundo o criterio crit.
 * Retorna negativo se a < b, zero se iguais, positivo se a > b.
 */
int sort_compare(Shape a, Shape b, SortCrit crit);

/*
 * Os seis algoritmos. Todos seguem a mesma assinatura:
 *   lst      : lista de trabalho (modificada in-place)
 *   k        : numero de menores elementos desejados (1 <= k <= size)
 *   crit     : criterio de comparacao
 *   anim     : callback de animacao (pode ser NULL para desativar)
 *   userdata : passado diretamente para anim
 */
void sort_selection(List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);
void sort_bubble   (List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);
void sort_insertion(List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);
void sort_shell    (List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);
void sort_quick    (List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);
void sort_merge    (List lst, int k, SortCrit crit,
                    AnimCallback anim, void *userdata);

#endif /* SORT_H */