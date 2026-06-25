#include "unity.h"
#include "list.h"
#include "shape.h"

void setUp(void)    {}
void tearDown(void) {}

void test_list_create_nao_null(void) {
    List l = list_create(4);
    TEST_ASSERT_NOT_NULL(l);
    list_destroy(l);
}

void test_list_size_inicial_zero(void) {
    List l = list_create(4);
    TEST_ASSERT_EQUAL_INT(0, list_size(l));
    list_destroy(l);
}

void test_list_append_e_get(void) {
    List l = list_create(4);
    Shape s = shape_create_circle(1, 0.0, 0.0, 1.0, "k", "k");
    list_append(l, s);
    TEST_ASSERT_EQUAL_INT(1, list_size(l));
    TEST_ASSERT_NOT_NULL(list_get(l, 0));
    list_destroy(l);
    shape_destroy(s);
}

void test_list_append_multiplos(void) {
    List l = list_create(4);
    Shape a = shape_create_circle(1, 0.0, 1.0, 1.0, "k", "k");
    Shape b = shape_create_circle(2, 0.0, 2.0, 1.0, "k", "k");
    Shape c = shape_create_circle(3, 0.0, 3.0, 1.0, "k", "k");
    list_append(l, a);
    list_append(l, b);
    list_append(l, c);
    TEST_ASSERT_EQUAL_INT(3, list_size(l));
    TEST_ASSERT_EQUAL_INT(1, shape_get_id(list_get(l, 0)));
    TEST_ASSERT_EQUAL_INT(2, shape_get_id(list_get(l, 1)));
    TEST_ASSERT_EQUAL_INT(3, shape_get_id(list_get(l, 2)));
    list_destroy(l);
    shape_destroy(a); shape_destroy(b); shape_destroy(c);
}

void test_list_cresce_alem_da_capacidade(void) {
    List l = list_create(2);
    int i;
    for (i = 0; i < 50; i++) {
        Shape s = shape_create_circle(i + 1, 0.0, (double)i, 1.0, "k", "k");
        list_append(l, s);
    }
    TEST_ASSERT_EQUAL_INT(50, list_size(l));
    for (i = 0; i < 50; i++) shape_destroy(list_get(l, i));
    list_destroy(l);
}

void test_list_swap(void) {
    List l = list_create(4);
    Shape a = shape_create_circle(1, 0.0, 0.0, 1.0, "k", "k");
    Shape b = shape_create_circle(2, 0.0, 1.0, 1.0, "k", "k");
    Shape c = shape_create_circle(3, 0.0, 2.0, 1.0, "k", "k");
    list_append(l, a);
    list_append(l, b);
    list_append(l, c);
    list_swap(l, 0, 2);
    TEST_ASSERT_EQUAL_INT(3, shape_get_id(list_get(l, 0)));
    TEST_ASSERT_EQUAL_INT(2, shape_get_id(list_get(l, 1)));
    TEST_ASSERT_EQUAL_INT(1, shape_get_id(list_get(l, 2)));
    list_destroy(l);
    shape_destroy(a); shape_destroy(b); shape_destroy(c);
}

void test_list_set(void) {
    List l = list_create(4);
    Shape a = shape_create_circle(1, 0.0, 0.0, 1.0, "k", "k");
    Shape b = shape_create_circle(2, 0.0, 1.0, 1.0, "k", "k");
    list_append(l, a);
    list_set(l, 0, b);
    TEST_ASSERT_EQUAL_INT(2, shape_get_id(list_get(l, 0)));
    list_destroy(l);
    shape_destroy(a); shape_destroy(b);
}

void test_list_clear(void) {
    List l = list_create(4);
    Shape s = shape_create_circle(1, 0.0, 0.0, 1.0, "k", "k");
    list_append(l, s);
    list_clear(l);
    TEST_ASSERT_EQUAL_INT(0, list_size(l));
    list_destroy(l);
    shape_destroy(s);
}

void test_list_swap_mesmo_indice(void) {
    List l = list_create(4);
    Shape a = shape_create_circle(7, 0.0, 0.0, 1.0, "k", "k");
    list_append(l, a);
    list_swap(l, 0, 0);
    TEST_ASSERT_EQUAL_INT(7, shape_get_id(list_get(l, 0)));
    list_destroy(l);
    shape_destroy(a);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_list_create_nao_null);
    RUN_TEST(test_list_size_inicial_zero);
    RUN_TEST(test_list_append_e_get);
    RUN_TEST(test_list_append_multiplos);
    RUN_TEST(test_list_cresce_alem_da_capacidade);
    RUN_TEST(test_list_swap);
    RUN_TEST(test_list_set);
    RUN_TEST(test_list_clear);
    RUN_TEST(test_list_swap_mesmo_indice);
    return UNITY_END();
}