#ifndef SVG_H
#define SVG_H

#include "shape.h"
#include "bst.h"
#include "list.h"
#include <stdio.h>

typedef void *SVGFile;

SVGFile svg_open(const char *filepath, double width, double height);
void    svg_close(SVGFile f);
void    svg_write_shape(SVGFile f, Shape s);
void    svg_write_selection_rect(SVGFile f, double x, double y, double w, double h);
void    svg_write_anchor_marker(SVGFile f, Shape s);
void    svg_write_bst(SVGFile f, BST b);
void    svg_write_list(SVGFile f, List lst);
FILE   *svg_get_fp(SVGFile f);

#endif