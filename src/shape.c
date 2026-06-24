#include "shape.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
 

typedef struct {
    int       id;
    ShapeType type;
    double    x, y;
    char      corb[64];
    char      corp[64];
    union {
        struct { double r; }                   circle;
        struct { double w, h; }                rect;
        struct { double x2, y2; }              line;
        struct { char anchor; char txto[512]; } text;
    } d;
} ShapeData;

/* ------------------------------------------------------------------ */
/* Estado global de estilo de texto                                    */
/* ------------------------------------------------------------------ */

static char   g_font_family[64] = "sans-serif";
static char   g_font_weight[32] = "normal";
static double g_font_size       = 12.0;

void shape_set_text_style(const char *fFamily, const char *fWeight, double fSize) {
    assert(fFamily != NULL);
    assert(fWeight != NULL);

    if      (strcmp(fFamily, "sans")    == 0) strncpy(g_font_family, "sans-serif", 63);
    else if (strcmp(fFamily, "serif")   == 0) strncpy(g_font_family, "serif",      63);
    else if (strcmp(fFamily, "cursive") == 0) strncpy(g_font_family, "cursive",    63);
    else                                       strncpy(g_font_family, fFamily,      63);
    g_font_family[63] = '\0';

    if      (strcmp(fWeight, "n")  == 0) strncpy(g_font_weight, "normal",  31);
    else if (strcmp(fWeight, "b")  == 0) strncpy(g_font_weight, "bold",    31);
    else if (strcmp(fWeight, "b+") == 0) strncpy(g_font_weight, "bolder",  31);
    else if (strcmp(fWeight, "l")  == 0) strncpy(g_font_weight, "lighter", 31);
    else                                  strncpy(g_font_weight, fWeight,   31);
    g_font_weight[31] = '\0';

    g_font_size = fSize;
}

/* ------------------------------------------------------------------ */
/* Construtores                                                        */
/* ------------------------------------------------------------------ */

static ShapeData *alloc_shape(int id, ShapeType type,
                               double x, double y,
                               const char *corb, const char *corp) {
    ShapeData *sd = (ShapeData *)malloc(sizeof(ShapeData));
    if (!sd) return NULL;
    sd->id   = id;
    sd->type = type;
    sd->x    = x;
    sd->y    = y;
    strncpy(sd->corb, corb ? corb : "black", 63); sd->corb[63] = '\0';
    strncpy(sd->corp, corp ? corp : "black", 63); sd->corp[63] = '\0';
    return sd;
}

Shape shape_create_circle(int id, double x, double y, double r,
                           const char *corb, const char *corp) {
    ShapeData *sd = alloc_shape(id, SHAPE_CIRCLE, x, y, corb, corp);
    if (!sd) return NULL;
    sd->d.circle.r = r;
    return (Shape)sd;
}

Shape shape_create_rect(int id, double x, double y, double w, double h,
                         const char *corb, const char *corp) {
    ShapeData *sd = alloc_shape(id, SHAPE_RECT, x, y, corb, corp);
    if (!sd) return NULL;
    sd->d.rect.w = w;
    sd->d.rect.h = h;
    return (Shape)sd;
}

Shape shape_create_line(int id, double x1, double y1, double x2, double y2,
                         const char *cor) {
    ShapeData *sd = alloc_shape(id, SHAPE_LINE, x1, y1, cor, cor);
    if (!sd) return NULL;
    sd->d.line.x2 = x2;
    sd->d.line.y2 = y2;
    return (Shape)sd;
}

Shape shape_create_text(int id, double x, double y,
                         const char *corb, const char *corp,
                         char anchor, const char *txto) {
    ShapeData *sd = alloc_shape(id, SHAPE_TEXT, x, y, corb, corp);
    if (!sd) return NULL;
    sd->d.text.anchor = anchor;
    strncpy(sd->d.text.txto, txto ? txto : "", 511);
    sd->d.text.txto[511] = '\0';
    return (Shape)sd;
}

/* ------------------------------------------------------------------ */
/* Destrutor e clone                                                   */
/* ------------------------------------------------------------------ */

void shape_destroy(Shape s) {
    free(s);
}

Shape shape_clone(Shape s) {
    assert(s != NULL);
    ShapeData *orig  = (ShapeData *)s;
    ShapeData *clone = (ShapeData *)malloc(sizeof(ShapeData));
    if (!clone) return NULL;
    memcpy(clone, orig, sizeof(ShapeData));
    return (Shape)clone;
}

void shape_translate(Shape s, double dx, double dy) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    sd->x += dx;
    sd->y += dy;
    if (sd->type == SHAPE_LINE) {
        sd->d.line.x2 += dx;
        sd->d.line.y2 += dy;
    }
}

/* ------------------------------------------------------------------ */
/* Acessores basicos                                                   */
/* ------------------------------------------------------------------ */

int shape_get_id(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->id;
}

ShapeType shape_get_type(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->type;
}

double shape_get_x(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->x;
}

double shape_get_y(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->y;
}

/* ------------------------------------------------------------------ */
/* Dimensoes                                                           */
/* ------------------------------------------------------------------ */

double shape_get_width(Shape s) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    switch (sd->type) {
        case SHAPE_CIRCLE: return 2.0 * sd->d.circle.r;
        case SHAPE_RECT:   return sd->d.rect.w;
        case SHAPE_LINE:   return 0.0;
        case SHAPE_TEXT:   return 1.0 * (double)strlen(sd->d.text.txto);
        default: assert(0 && "tipo desconhecido em shape_get_width");
    }
    return 0.0;
}

double shape_get_height(Shape s) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    switch (sd->type) {
        case SHAPE_CIRCLE: return 2.0 * sd->d.circle.r;
        case SHAPE_RECT:   return sd->d.rect.h;
        case SHAPE_LINE:   return 1.5;
        case SHAPE_TEXT:   return 10.0;
        default: assert(0 && "tipo desconhecido em shape_get_height");
    }
    return 0.0;
}

double shape_get_area(Shape s) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    switch (sd->type) {
        case SHAPE_CIRCLE: return M_PI * sd->d.circle.r * sd->d.circle.r;
        case SHAPE_RECT:   return sd->d.rect.w * sd->d.rect.h;
        case SHAPE_LINE: {
            double dx = sd->d.line.x2 - sd->x;
            double dy = sd->d.line.y2 - sd->y;
            return 1.5 * sqrt(dx * dx + dy * dy);
        }
        case SHAPE_TEXT:
            return 10.0 * (double)strlen(sd->d.text.txto);
        default: assert(0 && "tipo desconhecido em shape_get_area");
    }
    return 0.0;
}

/* ------------------------------------------------------------------ */
/* Cores                                                               */
/* ------------------------------------------------------------------ */

const char *shape_get_fill_color(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->corp;
}

const char *shape_get_border_color(Shape s) {
    assert(s != NULL);
    return ((ShapeData *)s)->corb;
}

void shape_set_colors(Shape s, const char *corb, const char *corp) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    if (corb) { strncpy(sd->corb, corb, 63); sd->corb[63] = '\0'; }
    if (corp) { strncpy(sd->corp, corp, 63); sd->corp[63] = '\0'; }
}

/* ------------------------------------------------------------------ */
/* Bounding box e contencao                                            */
/* ------------------------------------------------------------------ */

static void bounding_box(ShapeData *sd,
                          double *bx, double *by, double *bw, double *bh) {
    switch (sd->type) {
        case SHAPE_CIRCLE:
            *bx = sd->x - sd->d.circle.r;
            *by = sd->y - sd->d.circle.r;
            *bw = 2.0 * sd->d.circle.r;
            *bh = 2.0 * sd->d.circle.r;
            break;
        case SHAPE_RECT:
            *bx = sd->x;
            *by = sd->y - sd->d.rect.h;  /* ancora e canto inferior-esquerdo */
            *bw = sd->d.rect.w;
            *bh = sd->d.rect.h;
            break;
        case SHAPE_LINE: {
            double x2 = sd->d.line.x2, y2 = sd->d.line.y2;
            *bx = fmin(sd->x, x2);
            *by = fmin(sd->y, y2);
            *bw = fabs(x2 - sd->x);
            *bh = fabs(y2 - sd->y);
            break;
        }
        case SHAPE_TEXT:
            *bx = sd->x;
            *by = sd->y;
            *bw = 1.0 * (double)strlen(sd->d.text.txto);
            *bh = 10.0;
            break;
        default:
            assert(0 && "tipo desconhecido em bounding_box");
    }
}

int shape_inside_rect(Shape s, double rx, double ry, double rw, double rh) {
    assert(s != NULL);
    ShapeData *sd = (ShapeData *)s;
    double bx, by, bw, bh;
    bounding_box(sd, &bx, &by, &bw, &bh);
    return (bx >= rx) && (by >= ry) &&
           (bx + bw <= rx + rw) && (by + bh <= ry + rh);
}

/* ------------------------------------------------------------------ */
/* Comparacao default (y, x, area)                                     */
/* ------------------------------------------------------------------ */

static int cmp_double(double a, double b) {
    if (fabs(a - b) < 1e-9) return 0;
    return (a < b) ? -1 : 1;
}

int shape_compare_default(Shape a, Shape b) {
    assert(a != NULL && b != NULL);
    ShapeData *sa = (ShapeData *)a;
    ShapeData *sb = (ShapeData *)b;
    int c;
    c = cmp_double(sa->y, sb->y); if (c != 0) return c;
    c = cmp_double(sa->x, sb->x); if (c != 0) return c;
    return cmp_double(shape_get_area(a), shape_get_area(b));
}

/* ------------------------------------------------------------------ */
/* Emissao SVG                                                         */
/* ------------------------------------------------------------------ */

void shape_write_svg(Shape s, FILE *fp) {
    assert(s != NULL && fp != NULL);
    ShapeData *sd = (ShapeData *)s;
    switch (sd->type) {
        case SHAPE_CIRCLE:
            fprintf(fp,
                "  <circle cx=\"%.4f\" cy=\"%.4f\" r=\"%.4f\""
                " stroke=\"%s\" fill=\"%s\" stroke-width=\"1\"/>\n",
                sd->x, sd->y, sd->d.circle.r, sd->corb, sd->corp);
            break;
        case SHAPE_RECT: {
            double svg_y = sd->y - sd->d.rect.h;
            fprintf(fp,
                "  <rect x=\"%.4f\" y=\"%.4f\" width=\"%.4f\" height=\"%.4f\""
                " stroke=\"%s\" fill=\"%s\" stroke-width=\"1\"/>\n",
                sd->x, svg_y, sd->d.rect.w, sd->d.rect.h, sd->corb, sd->corp);
            break;
        }
        case SHAPE_LINE:
            fprintf(fp,
                "  <line x1=\"%.4f\" y1=\"%.4f\" x2=\"%.4f\" y2=\"%.4f\""
                " stroke=\"%s\" stroke-width=\"1\"/>\n",
                sd->x, sd->y, sd->d.line.x2, sd->d.line.y2, sd->corb);
            break;
        case SHAPE_TEXT: {
            const char *anchor_str = "start";
            if (sd->d.text.anchor == 'm') anchor_str = "middle";
            else if (sd->d.text.anchor == 'f') anchor_str = "end";
            fprintf(fp,
                "  <text x=\"%.4f\" y=\"%.4f\""
                " font-family=\"%s\" font-weight=\"%s\" font-size=\"%.1f\""
                " text-anchor=\"%s\" stroke=\"%s\" fill=\"%s\">%s</text>\n",
                sd->x, sd->y,
                g_font_family, g_font_weight, g_font_size,
                anchor_str, sd->corb, sd->corp, sd->d.text.txto);
            break;
        }
        default:
            assert(0 && "tipo desconhecido em shape_write_svg");
    }
}