#include "unity.h"
#include "shape.h"
#include <stdio.h>
#include <math.h>

void setUp(void)    {}
void tearDown(void) {}

/* --- areas --- */

void test_circulo_area(void) {
    Shape s = shape_create_circle(1, 0.0, 0.0, 5.0, "black", "red");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 78.5398, shape_get_area(s));
    shape_destroy(s);
}

void test_retangulo_area(void) {
    Shape s = shape_create_rect(2, 0.0, 10.0, 4.0, 3.0, "black", "blue");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 12.0, shape_get_area(s));
    shape_destroy(s);
}

void test_linha_area(void) {
    /* linha (0,0)-(3,4): comprimento=5, area=1.5*5=7.5 */
    Shape s = shape_create_line(3, 0.0, 0.0, 3.0, 4.0, "black");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 7.5, shape_get_area(s));
    shape_destroy(s);
}

void test_texto_area(void) {
    /* "hello" = 5 chars, area = 10.0 * 5 = 50.0 */
    Shape s = shape_create_text(4, 0.0, 0.0, "black", "white", 'i', "hello");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 50.0, shape_get_area(s));
    shape_destroy(s);
}

/* --- dimensoes especiais --- */

void test_linha_altura(void) {
    Shape s = shape_create_line(5, 0.0, 0.0, 10.0, 10.0, "black");
    TEST_ASSERT_FLOAT_WITHIN(0.001, 1.5, shape_get_height(s));
    shape_destroy(s);
}

void test_texto_dimensoes(void) {
    Shape s = shape_create_text(6, 0.0, 0.0, "black", "white", 'm', "abc");
    TEST_ASSERT_FLOAT_WITHIN(0.001, 3.0,  shape_get_width(s));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, shape_get_height(s));
    shape_destroy(s);
}

/* --- criacao retorna ponteiro valido --- */

void test_create_circulo_nao_null(void) {
    Shape s = shape_create_circle(7, 1.0, 2.0, 3.0, "red", "green");
    TEST_ASSERT_NOT_NULL(s);
    shape_destroy(s);
}

void test_create_linha_nao_null(void) {
    Shape s = shape_create_line(8, 0.0, 0.0, 5.0, 5.0, "blue");
    TEST_ASSERT_NOT_NULL(s);
    shape_destroy(s);
}

/* --- contencao --- */

void test_circulo_dentro_regiao(void) {
    /* circulo centro (50,50) r=5, cabe em (40,40,30,30) */
    Shape s = shape_create_circle(9, 50.0, 50.0, 5.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(1, shape_inside_rect(s, 40.0, 40.0, 30.0, 30.0));
    shape_destroy(s);
}

void test_circulo_fora_regiao(void) {
    /* circulo centro (50,50) r=5 extravasa (0,0,52,52) */
    Shape s = shape_create_circle(10, 50.0, 50.0, 5.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(0, shape_inside_rect(s, 0.0, 0.0, 52.0, 52.0));
    shape_destroy(s);
}

void test_retangulo_dentro_regiao(void) {
    /* rect ancora (10,30) w=5 h=5 -> box x:[10,15] y:[25,30] cabe em (5,20,20,20) */
    Shape s = shape_create_rect(11, 10.0, 30.0, 5.0, 5.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(1, shape_inside_rect(s, 5.0, 20.0, 20.0, 20.0));
    shape_destroy(s);
}

/* --- comparacao default (y, x, area) --- */

void test_compare_por_y(void) {
    Shape a = shape_create_rect(12, 0.0, 10.0, 1.0, 1.0, "k", "k");
    Shape b = shape_create_rect(13, 0.0, 20.0, 1.0, 1.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(1, shape_compare_default(a, b) < 0);
    shape_destroy(a);
    shape_destroy(b);
}

void test_compare_empate_y_por_x(void) {
    Shape a = shape_create_circle(14, 5.0,  10.0, 1.0, "k", "k");
    Shape b = shape_create_circle(15, 10.0, 10.0, 1.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(1, shape_compare_default(a, b) < 0);
    shape_destroy(a);
    shape_destroy(b);
}

/* --- clone independencia --- */

void test_clone_independente(void) {
    Shape orig  = shape_create_circle(16, 10.0, 10.0, 5.0, "k", "k");
    Shape clone = shape_clone(orig);
    TEST_ASSERT_NOT_NULL(clone);
    shape_translate(orig, 100.0, 100.0);
    /* clone nao deve ter sido afetado */
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, shape_get_x(clone));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 10.0, shape_get_y(clone));
    shape_destroy(orig);
    shape_destroy(clone);
}

/* --- cores --- */

void test_get_fill_color(void) {
    Shape s = shape_create_circle(17, 0.0, 0.0, 1.0, "black", "crimson");
    TEST_ASSERT_EQUAL_STRING("crimson", shape_get_fill_color(s));
    shape_destroy(s);
}

void test_get_border_color(void) {
    Shape s = shape_create_rect(18, 0.0, 0.0, 1.0, 1.0, "navy", "white");
    TEST_ASSERT_EQUAL_STRING("navy", shape_get_border_color(s));
    shape_destroy(s);
}

void test_set_colors(void) {
    Shape s = shape_create_circle(19, 0.0, 0.0, 1.0, "black", "black");
    shape_set_colors(s, "red", "blue");
    TEST_ASSERT_EQUAL_STRING("red",  shape_get_border_color(s));
    TEST_ASSERT_EQUAL_STRING("blue", shape_get_fill_color(s));
    shape_destroy(s);
}

/* --- linha usa cor da borda como fill --- */

void test_linha_fill_color_e_borda(void) {
    Shape s = shape_create_line(20, 0.0, 0.0, 1.0, 1.0, "green");
    TEST_ASSERT_EQUAL_STRING("green", shape_get_fill_color(s));
    TEST_ASSERT_EQUAL_STRING("green", shape_get_border_color(s));
    shape_destroy(s);
}

/* --- SVG: abertura e fechamento sem crash (secao 4.5 do manual) --- */

void test_shape_write_svg_nao_crasha(void) {
    Shape s = shape_create_rect(21, 5.0, 20.0, 10.0, 5.0, "black", "blue");
    FILE *f = fopen("tmp_test_shape.svg", "w");
    TEST_ASSERT_NOT_NULL(f);
    shape_write_svg(s, f);
    fclose(f);
    remove("tmp_test_shape.svg");
    shape_destroy(s);
}

/* --- id --- */

void test_get_id(void) {
    Shape s = shape_create_circle(42, 0.0, 0.0, 1.0, "k", "k");
    TEST_ASSERT_EQUAL_INT(42, shape_get_id(s));
    shape_destroy(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_circulo_area);
    RUN_TEST(test_retangulo_area);
    RUN_TEST(test_linha_area);
    RUN_TEST(test_texto_area);
    RUN_TEST(test_linha_altura);
    RUN_TEST(test_texto_dimensoes);
    RUN_TEST(test_create_circulo_nao_null);
    RUN_TEST(test_create_linha_nao_null);
    RUN_TEST(test_circulo_dentro_regiao);
    RUN_TEST(test_circulo_fora_regiao);
    RUN_TEST(test_retangulo_dentro_regiao);
    RUN_TEST(test_compare_por_y);
    RUN_TEST(test_compare_empate_y_por_x);
    RUN_TEST(test_clone_independente);
    RUN_TEST(test_get_fill_color);
    RUN_TEST(test_get_border_color);
    RUN_TEST(test_set_colors);
    RUN_TEST(test_linha_fill_color_e_borda);
    RUN_TEST(test_shape_write_svg_nao_crasha);
    RUN_TEST(test_get_id);
    return UNITY_END();
}