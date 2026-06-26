#include "unity.h"
#include "sort.h"
#include "list.h"
#include "shape.h"

void setUp(void)    {}
void tearDown(void) {}

/* --- contador de frames --- */

typedef struct { int frames; } TestCtx;

static void anim_counter(List lst, int k, int frame, void *ud) {
    (void)lst; (void)k; (void)frame;
    ((TestCtx *)ud)->frames++;
}

/* --- helpers --- */

/* Cria lista com 5 retangulos de areas [5,3,1,4,2] */
static List make_area_list(void) {
    double areas[] = {5.0, 3.0, 1.0, 4.0, 2.0};
    List l = list_create(5);
    int i;
    for (i = 0; i < 5; i++) {
        Shape s = shape_create_rect(i + 1, 0.0, (double)i,
                                    areas[i], 1.0, "k", "k");
        list_append(l, s);
    }
    return l;
}

static void destroy_list_shapes(List l) {
    int i;
    for (i = 0; i < list_size(l); i++) shape_destroy(list_get(l, i));
    list_destroy(l);
}

/* Verifica que os primeiros k elementos sao menores que os restantes */
static int k_menores_ok(List l, int k, SortCrit crit) {
    int i, j;
    for (i = 0; i < k; i++)
        for (j = k; j < list_size(l); j++)
            if (sort_compare(list_get(l, i), list_get(l, j), crit) > 0)
                return 0;
    return 1;
}

/* --- testes de k menores por area para cada algoritmo --- */

void test_selection_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_selection(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

void test_bubble_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_bubble(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

void test_insertion_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_insertion(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

void test_shell_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_shell(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

void test_quick_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_quick(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

void test_merge_sort_k2(void) {
    List l = make_area_list();
    TestCtx ctx = {0};
    sort_merge(l, 2, CRIT_AREA, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    TEST_ASSERT_EQUAL_INT(1, ctx.frames > 0);
    destroy_list_shapes(l);
}

/* --- criterio por cor (ordenacao alfabetica) --- */

void test_sort_criterio_cor(void) {
    List l = list_create(3);
    list_append(l, shape_create_circle(1, 0.0, 0.0, 1.0, "k", "red"));
    list_append(l, shape_create_circle(2, 0.0, 1.0, 1.0, "k", "blue"));
    list_append(l, shape_create_circle(3, 0.0, 2.0, 1.0, "k", "green"));
    TestCtx ctx = {0};
    sort_selection(l, 3, CRIT_COLOR, anim_counter, &ctx);
    TEST_ASSERT_EQUAL_STRING("blue",  shape_get_fill_color(list_get(l, 0)));
    TEST_ASSERT_EQUAL_STRING("green", shape_get_fill_color(list_get(l, 1)));
    TEST_ASSERT_EQUAL_STRING("red",   shape_get_fill_color(list_get(l, 2)));
    destroy_list_shapes(l);
}

/* --- criterio default (y, x, area) --- */

void test_sort_criterio_default(void) {
    List l = list_create(3);
    list_append(l, shape_create_circle(1, 0.0, 30.0, 1.0, "k", "k"));
    list_append(l, shape_create_circle(2, 0.0, 10.0, 1.0, "k", "k"));
    list_append(l, shape_create_circle(3, 0.0, 20.0, 1.0, "k", "k"));
    TestCtx ctx = {0};
    sort_insertion(l, 3, CRIT_DEFAULT, anim_counter, &ctx);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, shape_get_y(list_get(l, 0)));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 20.0, shape_get_y(list_get(l, 1)));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 30.0, shape_get_y(list_get(l, 2)));
    destroy_list_shapes(l);
}

/* --- criterio altura --- */

void test_sort_criterio_altura(void) {
    List l = list_create(3);
    list_append(l, shape_create_rect(1, 0.0, 0.0, 1.0, 9.0, "k", "k"));
    list_append(l, shape_create_rect(2, 0.0, 0.0, 1.0, 3.0, "k", "k"));
    list_append(l, shape_create_rect(3, 0.0, 0.0, 1.0, 6.0, "k", "k"));
    TestCtx ctx = {0};
    sort_merge(l, 3, CRIT_HEIGHT, anim_counter, &ctx);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 3.0, shape_get_height(list_get(l, 0)));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 6.0, shape_get_height(list_get(l, 1)));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 9.0, shape_get_height(list_get(l, 2)));
    destroy_list_shapes(l);
}

/* --- parse de criterio --- */

void test_sort_parse_crit(void) {
    TEST_ASSERT_EQUAL_INT(CRIT_DEFAULT, sort_parse_crit("d"));
    TEST_ASSERT_EQUAL_INT(CRIT_AREA,    sort_parse_crit("a"));
    TEST_ASSERT_EQUAL_INT(CRIT_WIDTH,   sort_parse_crit("w"));
    TEST_ASSERT_EQUAL_INT(CRIT_HEIGHT,  sort_parse_crit("h"));
    TEST_ASSERT_EQUAL_INT(CRIT_COLOR,   sort_parse_crit("c"));
}

/* --- anim NULL nao crasha --- */

void test_sort_anim_null_nao_crasha(void) {
    List l = make_area_list();
    sort_selection(l, 2, CRIT_AREA, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(1, k_menores_ok(l, 2, CRIT_AREA));
    destroy_list_shapes(l);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_selection_sort_k2);
    RUN_TEST(test_bubble_sort_k2);
    RUN_TEST(test_insertion_sort_k2);
    RUN_TEST(test_shell_sort_k2);
    RUN_TEST(test_quick_sort_k2);
    RUN_TEST(test_merge_sort_k2);
    RUN_TEST(test_sort_criterio_cor);
    RUN_TEST(test_sort_criterio_default);
    RUN_TEST(test_sort_criterio_altura);
    RUN_TEST(test_sort_parse_crit);
    RUN_TEST(test_sort_anim_null_nao_crasha);
    return UNITY_END();
}