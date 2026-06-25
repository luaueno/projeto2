#include "unity.h"
#include "bst.h"
#include "shape.h"

void setUp(void)    {}
void tearDown(void) {}

/* --- criacao --- */

void test_bst_create_nao_null(void) {
    BST b = bst_create();
    TEST_ASSERT_NOT_NULL(b);
    bst_destroy(b);
}

void test_bst_size_inicial_zero(void) {
    BST b = bst_create();
    TEST_ASSERT_EQUAL_INT(0, bst_size(b));
    bst_destroy(b);
}

/* --- insercao --- */

void test_bst_insert_size(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 10.0, 5.0, "k", "k"));
    TEST_ASSERT_EQUAL_INT(1, bst_size(b));
    bst_destroy(b);
}

void test_bst_insert_tres(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 10.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(2, 0.0, 20.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(3, 0.0,  5.0, 1.0, "k", "k"));
    TEST_ASSERT_EQUAL_INT(3, bst_size(b));
    bst_destroy(b);
}

/* --- traversal em ordem --- */

static int   visit_count_g = 0;
static double last_y_g     = -1.0;

static void visitor_ordem(Shape s, void *ud) {
    (void)ud;
    double y = shape_get_y(s);
    TEST_ASSERT_EQUAL_INT(1, y >= last_y_g);
    last_y_g = y;
    visit_count_g++;
}

void test_bst_inorder_ordenado(void) {
    visit_count_g = 0;
    last_y_g      = -1.0;
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 5.0, 30.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(2, 5.0, 10.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(3, 5.0, 20.0, 1.0, "k", "k"));
    bst_inorder(b, visitor_ordem, NULL);
    TEST_ASSERT_EQUAL_INT(3, visit_count_g);
    bst_destroy(b);
}

void test_bst_inorder_cinco_elementos(void) {
    visit_count_g = 0;
    last_y_g      = -1.0;
    BST b = bst_create();
    double ys[] = {50.0, 10.0, 40.0, 20.0, 30.0};
    int i;
    for (i = 0; i < 5; i++)
        bst_insert(b, shape_create_circle(i + 1, 0.0, ys[i], 1.0, "k", "k"));
    bst_inorder(b, visitor_ordem, NULL);
    TEST_ASSERT_EQUAL_INT(5, visit_count_g);
    bst_destroy(b);
}

/* --- remocao --- */

void test_bst_remove_by_id(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 10.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(2, 0.0, 20.0, 1.0, "k", "k"));
    bst_insert(b, shape_create_circle(3, 0.0, 30.0, 1.0, "k", "k"));
    bst_remove_by_id(b, 2);
    TEST_ASSERT_EQUAL_INT(2, bst_size(b));
    bst_destroy(b);
}

void test_bst_remove_raiz(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 10.0, 1.0, "k", "k"));
    bst_remove_by_id(b, 1);
    TEST_ASSERT_EQUAL_INT(0, bst_size(b));
    bst_destroy(b);
}

/* caminho infeliz: ID inexistente nao crasha (secao 5 do manual) */
void test_bst_remove_id_inexistente(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 10.0, 1.0, "k", "k"));
    bst_remove_by_id(b, 999);
    TEST_ASSERT_EQUAL_INT(1, bst_size(b));
    bst_destroy(b);
}

void test_bst_remove_em_arvore_vazia(void) {
    BST b = bst_create();
    bst_remove_by_id(b, 99);
    TEST_ASSERT_EQUAL_INT(0, bst_size(b));
    bst_destroy(b);
}

/* --- busca por id --- */

void test_bst_find_by_id_encontra(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(42, 0.0, 0.0, 1.0, "k", "k"));
    Shape found = bst_find_by_id(b, 42);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(42, shape_get_id(found));
    bst_destroy(b);
}

void test_bst_find_by_id_retorna_null(void) {
    BST b = bst_create();
    bst_insert(b, shape_create_circle(1, 0.0, 0.0, 1.0, "k", "k"));
    Shape found = bst_find_by_id(b, 999);
    TEST_ASSERT_NULL(found);
    bst_destroy(b);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bst_create_nao_null);
    RUN_TEST(test_bst_size_inicial_zero);
    RUN_TEST(test_bst_insert_size);
    RUN_TEST(test_bst_insert_tres);
    RUN_TEST(test_bst_inorder_ordenado);
    RUN_TEST(test_bst_inorder_cinco_elementos);
    RUN_TEST(test_bst_remove_by_id);
    RUN_TEST(test_bst_remove_raiz);
    RUN_TEST(test_bst_remove_id_inexistente);
    RUN_TEST(test_bst_remove_em_arvore_vazia);
    RUN_TEST(test_bst_find_by_id_encontra);
    RUN_TEST(test_bst_find_by_id_retorna_null);
    return UNITY_END();
}