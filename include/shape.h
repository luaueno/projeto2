#ifndef SHAPE_H
#define SHAPE_H

#include <stdio.h>

typedef void *Shape;

typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECT,
    SHAPE_LINE,
    SHAPE_TEXT
} ShapeType;

/* construtores — retornam NULL em falha de alocacao */
Shape shape_create_circle(int id, double x, double y, double r,
                           const char *corb, const char *corp);
Shape shape_create_rect(int id, double x, double y, double w, double h,
                         const char *corb, const char *corp);
Shape shape_create_line(int id, double x1, double y1, double x2, double y2,
                         const char *cor);
Shape shape_create_text(int id, double x, double y,
                         const char *corb, const char *corp,
                         char anchor, const char *txto);

void  shape_destroy(Shape s);
Shape shape_clone(Shape s);
void  shape_translate(Shape s, double dx, double dy);

int       shape_get_id(Shape s);
ShapeType shape_get_type(Shape s);
double    shape_get_x(Shape s);
double    shape_get_y(Shape s);

/*
 * Dimensoes para ordenacao:
 *   Circulo  : 2r / 2r / pi*r^2
 *   Retangulo: w / h / w*h
 *   Linha    : 0 / 1.5 / 1.5*comprimento
 *   Texto    : 1.0*nchars / 10.0 / 10.0*nchars
 */
double shape_get_width(Shape s);
double shape_get_height(Shape s);
double shape_get_area(Shape s);

const char *shape_get_fill_color(Shape s);
const char *shape_get_border_color(Shape s);
void shape_set_colors(Shape s, const char *corb, const char *corp);

/* retorna 1 se a bounding box da forma esta inteiramente dentro do retangulo */
int shape_inside_rect(Shape s, double rx, double ry, double rw, double rh);

/* compara por y, depois x, depois area — retorna <0, 0 ou >0 */
int shape_compare_default(Shape a, Shape b);

void shape_write_svg(Shape s, FILE *fp);

/* estilo global aplicado a shapes de texto criadas apos essa chamada */
void shape_set_text_style(const char *fFamily, const char *fWeight, double fSize);

#endif