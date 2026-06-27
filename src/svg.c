#include "svg.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct {
    FILE  *fp;
    double width;
    double height;
} SVGFileData;

/* ------------------------------------------------------------------ */
/* Auxiliar para traversal da BST                                      */
/* ------------------------------------------------------------------ */

static void write_shape_visitor(Shape s, void *ud) {
    svg_write_shape((SVGFile)ud, s);
}

/* ------------------------------------------------------------------ */
/* Interface publica                                                    */
/* ------------------------------------------------------------------ */

SVGFile svg_open(const char *filepath, double width, double height) {
    assert(filepath != NULL);
    FILE *fp = fopen(filepath, "w");
    if (!fp) return NULL;

    SVGFileData *sf = (SVGFileData *)malloc(sizeof(SVGFileData));
    if (!sf) { fclose(fp); return NULL; }
    sf->fp     = fp;
    sf->width  = width;
    sf->height = height;

    fprintf(fp,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg xmlns=\"http://www.w3.org/2000/svg\""
        " width=\"%.4f\" height=\"%.4f\""
        " viewBox=\"0 0 %.4f %.4f\">\n",
        width, height, width, height);

    return (SVGFile)sf;
}

void svg_close(SVGFile f) {
    assert(f != NULL);
    SVGFileData *sf = (SVGFileData *)f;
    fprintf(sf->fp, "</svg>\n");
    fclose(sf->fp);
    free(sf);
}

void svg_write_shape(SVGFile f, Shape s) {
    assert(f != NULL && s != NULL);
    shape_write_svg(s, ((SVGFileData *)f)->fp);
}

void svg_write_selection_rect(SVGFile f, double x, double y,
                               double w, double h) {
    assert(f != NULL);
    fprintf(((SVGFileData *)f)->fp,
        "  <rect x=\"%.4f\" y=\"%.4f\" width=\"%.4f\" height=\"%.4f\""
        " stroke=\"red\" stroke-dasharray=\"5,5\""
        " fill=\"none\" stroke-width=\"2\"/>\n",
        x, y, w, h);
}

void svg_write_anchor_marker(SVGFile f, Shape s) {
    assert(f != NULL && s != NULL);
    double ax = shape_get_x(s);
    double ay = shape_get_y(s);
    fprintf(((SVGFileData *)f)->fp,
        "  <rect x=\"%.4f\" y=\"%.4f\" width=\"6\" height=\"6\""
        " fill=\"red\" stroke=\"none\"/>\n",
        ax - 3.0, ay - 3.0);
}

void svg_write_bst(SVGFile f, BST b) {
    assert(f != NULL && b != NULL);
    bst_inorder(b, write_shape_visitor, f);
}

void svg_write_list(SVGFile f, List lst) {
    assert(f != NULL && lst != NULL);
    int i;
    for (i = 0; i < list_size(lst); i++)
        svg_write_shape(f, list_get(lst, i));
}

FILE *svg_get_fp(SVGFile f) {
    assert(f != NULL);
    return ((SVGFileData *)f)->fp;
}