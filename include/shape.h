#ifndef SHAPE_H
#define SHAPE_H

#include <stdio.h>

/*
 * shape.h - Interface publica do modulo de formas geometricas.
 *
 * Cada forma e identificada por um inteiro (id) e possui uma coordenada
 * ancora, dimensoes e cores. O tipo Shape e um ponteiro opaco: a struct
 * interna e definida apenas em shape.c.
 *
 * Sistema de coordenadas: (0,0) no canto superior esquerdo, x cresce para
 * a direita e y cresce para baixo (identico ao SVG).
 *
 * Tipos suportados: circulo, retangulo, linha e texto.
 */

typedef void *Shape;

typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECT,
    SHAPE_LINE,
    SHAPE_TEXT
} ShapeType;

/*
 * Construtores — retornam NULL em caso de falha de alocacao.
 *
 * shape_create_circle : centro (x,y), raio r.
 * shape_create_rect   : ancora no canto inferior-esquerdo (x,y), largura w, altura h.
 * shape_create_line   : extremidades (x1,y1) e (x2,y2).
 * shape_create_text   : ancora (x,y), posicao da ancora a={'i','m','f'}, texto txto.
 *
 * corb = cor da borda; corp = cor do preenchimento (linha usa so corb).
 */
Shape shape_create_circle(int id, double x, double y, double r,
                           const char *corb, const char *corp);
Shape shape_create_rect(int id, double x, double y, double w, double h,
                         const char *corb, const char *corp);
Shape shape_create_line(int id, double x1, double y1, double x2, double y2,
                         const char *cor);
Shape shape_create_text(int id, double x, double y,
                         const char *corb, const char *corp,
                         char anchor, const char *txto);

/* Libera a memoria da forma. */
void shape_destroy(Shape s);

/* Copia profunda da forma. Retorna NULL em falha. */
Shape shape_clone(Shape s);

/* Desloca a ancora da forma por (dx, dy). */
void shape_translate(Shape s, double dx, double dy);

/* --- Acessores basicos --- */
int       shape_get_id(Shape s);
ShapeType shape_get_type(Shape s);
double    shape_get_x(Shape s);
double    shape_get_y(Shape s);

/*
 * Dimensoes para fins de ordenacao e consulta:
 *
 *   Circulo  : width  = 2r,  height = 2r,  area = pi*r^2
 *   Retangulo: width  = w,   height = h,   area = w*h
 *   Linha    : width  = 0,   height = 1.5, area = 1.5 * comprimento
 *   Texto    : width  = 1.0 * nchars, height = 10.0, area = 10.0 * nchars
 */
double shape_get_width(Shape s);
double shape_get_height(Shape s);
double shape_get_area(Shape s);

/* Retorna cor de preenchimento (para linha, retorna cor da borda). */
const char *shape_get_fill_color(Shape s);
const char *shape_get_border_color(Shape s);

/* Altera cores da forma. */
void shape_set_colors(Shape s, const char *corb, const char *corp);

/*
 * Verifica se a bounding box da forma esta inteiramente contida no
 * retangulo definido por (rx, ry, rw, rh).
 * Retorna 1 se sim, 0 caso contrario.
 */
int shape_inside_rect(Shape s, double rx, double ry, double rw, double rh);

/*
 * Comparacao pela ordem default: y crescente, depois x, depois area.
 * Retorna negativo se a < b, zero se iguais, positivo se a > b.
 */
int shape_compare_default(Shape a, Shape b);

/*
 * Emite o elemento SVG correspondente a forma no arquivo fp.
 * O retangulo usa ancora inferior-esquerda: SVG y = anchor_y - h.
 */
void shape_write_svg(Shape s, FILE *fp);

/*
 * Estilo global de texto, aplicado a todos os shape_create_text
 * subsequentes. Valores iniciais: sans-serif, normal, 12.0.
 *
 * fFamily : "sans" -> sans-serif | "serif" -> serif | "cursive" -> cursive
 * fWeight : "n" -> normal | "b" -> bold | "b+" -> bolder | "l" -> lighter
 * fSize   : tamanho em pontos
 */
void shape_set_text_style(const char *fFamily, const char *fWeight, double fSize);

#endif /* SHAPE_H */