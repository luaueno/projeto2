#ifndef SVG_H
#define SVG_H

#include "shape.h"
#include "bst.h"
#include "list.h"

/*
 * svg.h - Interface publica do gerador de arquivos SVG.
 *
 * SVGFile e um ponteiro opaco para um arquivo SVG aberto. Abre com
 * svg_open e fecha com svg_close. Entre as duas chamadas, use as
 * funcoes de escrita para adicionar elementos.
 */

typedef void *SVGFile;

/*
 * Abre o arquivo em filepath para escrita e emite o cabecalho SVG.
 * width e height definem o viewBox. Retorna NULL em caso de falha.
 */
SVGFile svg_open(const char *filepath, double width, double height);

/* Emite o rodape </svg> e fecha o arquivo. */
void svg_close(SVGFile f);

/* Emite o elemento SVG correspondente a shape s. */
void svg_write_shape(SVGFile f, Shape s);

/* Emite um retangulo com borda vermelha pontilhada (comando sel). */
void svg_write_selection_rect(SVGFile f, double x, double y,
                               double w, double h);

/*
 * Emite um quadrado vermelho 6x6 centrado na ancora de s
 * (marca os k menores no comando find).
 */
void svg_write_anchor_marker(SVGFile f, Shape s);

/* Emite todas as shapes da BST em ordem de traversal in-order. */
void svg_write_bst(SVGFile f, BST b);

/* Emite todas as shapes da lista, na ordem em que aparecem. */
void svg_write_list(SVGFile f, List lst);

/* Retorna o FILE* interno (usado por shape_write_svg diretamente). */
FILE *svg_get_fp(SVGFile f);

#endif /* SVG_H */