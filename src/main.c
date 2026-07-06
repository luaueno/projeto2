#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "shape.h"
#include "bst.h"
#include "list.h"
#include "sort.h"
#include "svg.h"

typedef struct {
    BST bst;
    const char *out_dir;
    char base_name[256];
    int *frame_counter;
    double svg_w, svg_h;
    int k;
} AnimCtx;

typedef struct { double max_x, max_y; } BBoxAcc;

static void bbox_visitor(Shape s, void *ud) {
    BBoxAcc *acc = (BBoxAcc *)ud;
    double ex = shape_get_x(s) + shape_get_width(s);
    double ey = shape_get_y(s) + shape_get_height(s);
    if (ex > acc->max_x) acc->max_x = ex;
    if (ey > acc->max_y) acc->max_y = ey;
}

static void calc_canvas(BST bst, double *out_w, double *out_h) {
    BBoxAcc acc = {200.0, 200.0};
    bst_inorder(bst, bbox_visitor, &acc);
    *out_w = acc.max_x + 20.0;
    *out_h = acc.max_y + 20.0;
}

static void anim_callback(List lst, int k, int frame, void *userdata) {
    (void)frame;
    AnimCtx *ctx = (AnimCtx *)userdata;
    char path[512];
    snprintf(path, sizeof(path), "%s/%s%06d.svg",
             ctx->out_dir, ctx->base_name, *ctx->frame_counter);
    (*ctx->frame_counter)++;

    SVGFile f = svg_open(path, ctx->svg_w, ctx->svg_h);
    if (!f) return;

    svg_write_bst(f, ctx->bst);

    int i;
    for (i = 0; i < list_size(lst); i++)
        svg_write_shape(f, list_get(lst, i));
    for (i = 0; i < k && i < list_size(lst); i++)
        svg_write_anchor_marker(f, list_get(lst, i));

    svg_close(f);
}

typedef struct { List sel; double rx, ry, rw, rh; } SelCtx;

static void sel_visitor(Shape s, void *ud) {
    SelCtx *ctx = (SelCtx *)ud;
    if (shape_inside_rect(s, ctx->rx, ctx->ry, ctx->rw, ctx->rh))
        list_append(ctx->sel, s);
}

static const char *type_str(ShapeType t) {
    switch (t) {
        case SHAPE_CIRCLE: return "c";
        case SHAPE_RECT:   return "r";
        case SHAPE_LINE:   return "l";
        case SHAPE_TEXT:   return "t";
        default:           return "?";
    }
}

static void print_shape_attr(FILE *fp, Shape s, SortCrit crit) {
    switch (crit) {
        case CRIT_AREA:
            fprintf(fp, "area=%.4f", shape_get_area(s));
            break;
        case CRIT_WIDTH:
            fprintf(fp, "largura=%.4f", shape_get_width(s));
            break;
        case CRIT_HEIGHT:
            fprintf(fp, "altura=%.4f", shape_get_height(s));
            break;
        case CRIT_COLOR:
            fprintf(fp, "cor=%s", shape_get_fill_color(s));
            break;
        case CRIT_DEFAULT:
            fprintf(fp, "y=%.4f x=%.4f area=%.4f",
                    shape_get_y(s), shape_get_x(s), shape_get_area(s));
            break;
        default: break;
    }
}

/* retira diretorio e extensao do caminho, ex: "input/arq.geo" -> "arq" */
static void strip_ext(const char *name, char *out, int out_sz) {
    const char *slash = strrchr(name, '/');
    if (!slash) slash = strrchr(name, '\\');
    const char *base = slash ? slash + 1 : name;

    strncpy(out, base, (size_t)(out_sz - 1));
    out[out_sz - 1] = '\0';
    char *dot = strrchr(out, '.');
    if (dot) *dot = '\0';
}

static void parse_geo(const char *path, BST bst) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", path);
        return;
    }

    char cmd[16];
    while (fscanf(fp, " %15s", cmd) == 1) {
        if (strcmp(cmd, "c") == 0) {
            int id; double x, y, r; char corb[64], corp[64];
            if (fscanf(fp, "%d %lf %lf %lf %63s %63s",
                       &id, &x, &y, &r, corb, corp) == 6)
                bst_insert(bst, shape_create_circle(id, x, y, r, corb, corp));

        } else if (strcmp(cmd, "r") == 0) {
            int id; double x, y, w, h; char corb[64], corp[64];
            if (fscanf(fp, "%d %lf %lf %lf %lf %63s %63s",
                       &id, &x, &y, &w, &h, corb, corp) == 7)
                bst_insert(bst, shape_create_rect(id, x, y, w, h, corb, corp));

        } else if (strcmp(cmd, "l") == 0) {
            int id; double x1, y1, x2, y2; char cor[64];
            if (fscanf(fp, "%d %lf %lf %lf %lf %63s",
                       &id, &x1, &y1, &x2, &y2, cor) == 6)
                bst_insert(bst, shape_create_line(id, x1, y1, x2, y2, cor));

        } else if (strcmp(cmd, "t") == 0) {
            int id; double x, y; char corb[64], corp[64], anchor_s[4];
            if (fscanf(fp, "%d %lf %lf %63s %63s %3s",
                       &id, &x, &y, corb, corp, anchor_s) == 6) {
                char txto[512];
                /* o texto vai ate o fim da linha, consome o espaco antes */
                int ch = fgetc(fp);
                if (ch == ' ') {
                    if (!fgets(txto, sizeof(txto), fp)) txto[0] = '\0';
                    txto[strcspn(txto, "\r\n")] = '\0';
                } else {
                    txto[0] = '\0';
                    if (ch != EOF) ungetc(ch, fp);
                }
                bst_insert(bst, shape_create_text(id, x, y, corb, corp,
                                                   anchor_s[0], txto));
            }

        } else if (strcmp(cmd, "ts") == 0) {
            char fam[64], wgt[32]; double sz;
            if (fscanf(fp, "%63s %31s %lf", fam, wgt, &sz) == 3)
                shape_set_text_style(fam, wgt, sz);

        } else {
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
        }
    }
    fclose(fp);
}

static void dispatch_sort(const char *alg, List lst, int k, SortCrit crit,
                           AnimCallback cb, void *ud) {
    if      (strcmp(alg, "ss")  == 0) sort_selection(lst, k, crit, cb, ud);
    else if (strcmp(alg, "bs")  == 0) sort_bubble(lst, k, crit, cb, ud);
    else if (strcmp(alg, "is")  == 0) sort_insertion(lst, k, crit, cb, ud);
    else if (strcmp(alg, "shs") == 0) sort_shell(lst, k, crit, cb, ud);
    else if (strcmp(alg, "qs")  == 0) sort_quick(lst, k, crit, cb, ud);
    else if (strcmp(alg, "ms")  == 0) sort_merge(lst, k, crit, cb, ud);
    else sort_selection(lst, k, crit, cb, ud);
}

static void parse_qry(const char *path, BST bst, SVGFile main_svg,
                       const char *out_dir, const char *base_name,
                       double svg_w, double svg_h, FILE *txt_fp) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", path);
        return;
    }

    List selected = list_create(16);
    int frame_counter = 1;

    char cmd[16];
    while (fscanf(fp, " %15s", cmd) == 1) {

        if (strcmp(cmd, "sel") == 0) {
            double x, y, w, h;
            if (fscanf(fp, "%lf %lf %lf %lf", &x, &y, &w, &h) != 4) continue;

            list_clear(selected);
            SelCtx ctx = { selected, x, y, w, h };
            bst_inorder(bst, sel_visitor, &ctx);

            svg_write_selection_rect(main_svg, x, y, w, h);

            fprintf(txt_fp, "sel %.4f %.4f %.4f %.4f\n", x, y, w, h);
            int i;
            for (i = 0; i < list_size(selected); i++)
                fprintf(txt_fp, "  id=%d tipo=%s\n",
                        shape_get_id(list_get(selected, i)),
                        type_str(shape_get_type(list_get(selected, i))));

        } else if (strcmp(cmd, "find") == 0 || strcmp(cmd, "findrm") == 0) {
            int k; char alg[8], crit_s[4]; double x, y, dw;
            if (fscanf(fp, "%d %7s %3s %lf %lf %lf",
                       &k, alg, crit_s, &x, &y, &dw) != 6) continue;

            int is_findrm = (strcmp(cmd, "findrm") == 0);
            SortCrit crit = sort_parse_crit(crit_s);

            List work = list_create(list_size(selected) + 1);
            int i;
            for (i = 0; i < list_size(selected); i++)
                list_append(work, shape_clone(list_get(selected, i)));

            AnimCtx actx;
            actx.bst = bst;
            actx.out_dir = out_dir;
            actx.svg_w = svg_w;
            actx.svg_h = svg_h;
            actx.k = k;
            actx.frame_counter = &frame_counter;
            strncpy(actx.base_name, base_name, 255);
            actx.base_name[255] = '\0';

            dispatch_sort(alg, work, k, crit, anim_callback, &actx);

            int n_work = list_size(work);
            for (i = 0; i < k && i < n_work; i++) {
                Shape s = list_get(work, i);
                double nx = x + (double)i * dw;
                shape_translate(s, nx - shape_get_x(s), y - shape_get_y(s));
            }

            /* frame final com os k selecionados em suas novas posicoes */
            {
                char fpath[512];
                snprintf(fpath, sizeof(fpath), "%s/%s%06d.svg",
                         out_dir, base_name, frame_counter++);
                SVGFile ff = svg_open(fpath, svg_w, svg_h);
                if (ff) {
                    int j;
                    svg_write_bst(ff, bst);
                    for (j = 0; j < k && j < n_work; j++) {
                        svg_write_shape(ff, list_get(work, j));
                        svg_write_anchor_marker(ff, list_get(work, j));
                    }
                    svg_close(ff);
                }
            }

            if (is_findrm) {
                for (i = k; i < n_work; i++)
                    bst_remove_by_id(bst, shape_get_id(list_get(work, i)));
            }

            fprintf(txt_fp, "%s %d %s %s %.4f %.4f %.4f\n",
                    cmd, k, alg, crit_s, x, y, dw);
            for (i = 0; i < k && i < n_work; i++) {
                Shape s = list_get(work, i);
                fprintf(txt_fp, "  id=%d tipo=%s ",
                        shape_get_id(s), type_str(shape_get_type(s)));
                print_shape_attr(txt_fp, s, crit);
                fprintf(txt_fp, "\n");
            }

            for (i = 0; i < n_work; i++) shape_destroy(list_get(work, i));
            list_destroy(work);

        } else if (strcmp(cmd, "cm") == 0) {
            double x, y, w, h, dx, dy;
            if (fscanf(fp, "%lf %lf %lf %lf %lf %lf",
                       &x, &y, &w, &h, &dx, &dy) != 6) continue;

            list_clear(selected);
            SelCtx ctx = { selected, x, y, w, h };
            bst_inorder(bst, sel_visitor, &ctx);

            int i, n = list_size(selected);
            for (i = 0; i < n; i++) {
                Shape clone = shape_clone(list_get(selected, i));
                shape_translate(clone, dx, dy);
                bst_insert(bst, clone);
                list_append(selected, clone);
            }
            /* reposiciona ponteiros: selected passa a apontar so para os clones */
            for (i = 0; i < n; i++) list_set(selected, i, list_get(selected, n + i));
            {
                List tmp = list_create(n + 1);
                for (i = n; i < list_size(selected); i++)
                    list_append(tmp, list_get(selected, i));
                list_clear(selected);
                for (i = 0; i < list_size(tmp); i++)
                    list_append(selected, list_get(tmp, i));
                list_destroy(tmp);
            }
            fprintf(txt_fp, "cm %.4f %.4f %.4f %.4f %.4f %.4f\n",
                    x, y, w, h, dx, dy);

        } else if (strcmp(cmd, "mc") == 0) {
            char corb[64], corp[64];
            if (fscanf(fp, "%63s %63s", corb, corp) != 2) continue;
            int i;
            for (i = 0; i < list_size(selected); i++)
                shape_set_colors(list_get(selected, i), corb, corp);
            fprintf(txt_fp, "mc %s %s\n", corb, corp);

        } else {
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
        }
    }

    list_destroy(selected);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    const char *base_in = NULL;
    const char *geo_file = NULL;
    const char *out_dir = NULL;
    const char *qry_file = NULL;

    int i;
    for (i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-e") == 0 && i + 1 < argc) base_in  = argv[++i];
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) geo_file = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out_dir  = argv[++i];
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) qry_file = argv[++i];
    }

    if (!geo_file || !out_dir) {
        fprintf(stderr, "Uso: ted [-e dir] -f arq.geo -o dir [-q arq.qry]\n");
        return 1;
    }

    char geo_path[512];
    if (base_in)
        snprintf(geo_path, sizeof(geo_path), "%s/%s", base_in, geo_file);
    else
        snprintf(geo_path, sizeof(geo_path), "%s", geo_file);

    char geo_base[256];
    strip_ext(geo_file, geo_base, sizeof(geo_base));

    BST bst = bst_create();
    parse_geo(geo_path, bst);

    double svg_w, svg_h;
    calc_canvas(bst, &svg_w, &svg_h);

    char out_path[512];
    snprintf(out_path, sizeof(out_path), "%s/%s.svg", out_dir, geo_base);
    {
        SVGFile f = svg_open(out_path, svg_w, svg_h);
        if (f) { svg_write_bst(f, bst); svg_close(f); }
    }

    if (!qry_file) {
        bst_destroy(bst);
        return 0;
    }

    char qry_path[512];
    if (base_in)
        snprintf(qry_path, sizeof(qry_path), "%s/%s", base_in, qry_file);
    else
        snprintf(qry_path, sizeof(qry_path), "%s", qry_file);

    char qry_base[256];
    strip_ext(qry_file, qry_base, sizeof(qry_base));

    char arqcons[512];
    snprintf(arqcons, sizeof(arqcons), "%s-%s", geo_base, qry_base);

    char cons_svg[512];
    snprintf(cons_svg, sizeof(cons_svg), "%s/%s.svg", out_dir, arqcons);
    SVGFile main_svg = svg_open(cons_svg, svg_w, svg_h);
    if (!main_svg) { bst_destroy(bst); return 1; }

    char cons_txt[512];
    snprintf(cons_txt, sizeof(cons_txt), "%s/%s.txt", out_dir, arqcons);
    FILE *txt_fp = fopen(cons_txt, "w");
    if (!txt_fp) { svg_close(main_svg); bst_destroy(bst); return 1; }

    parse_qry(qry_path, bst, main_svg, out_dir, arqcons, svg_w, svg_h, txt_fp);

    svg_write_bst(main_svg, bst);
    svg_close(main_svg);

    fclose(txt_fp);
    bst_destroy(bst);
    return 0;
}