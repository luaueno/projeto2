#include "bst.h"
#include <stdlib.h>
#include <assert.h>

typedef struct BSTNode {
    Shape data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    int count;
} BSTData;

static BSTNode *node_create(Shape s) {
    BSTNode *n = (BSTNode *)malloc(sizeof(BSTNode));
    if (!n) return NULL;
    n->data  = s;
    n->left  = NULL;
    n->right = NULL;
    return n;
}

static BSTNode *insert_rec(BSTNode *node, Shape s, int *count) {
    if (!node) {
        (*count)++;
        return node_create(s);
    }
    if (shape_compare_default(s, node->data) <= 0)
        node->left  = insert_rec(node->left,  s, count);
    else
        node->right = insert_rec(node->right, s, count);
    return node;
}

static void inorder_rec(BSTNode *node, BSTVisitor fn, void *ud) {
    if (!node) return;
    inorder_rec(node->left, fn, ud);
    fn(node->data, ud);
    inorder_rec(node->right, fn, ud);
}

static void destroy_rec(BSTNode *node) {
    if (!node) return;
    destroy_rec(node->left);
    destroy_rec(node->right);
    shape_destroy(node->data);
    free(node);
}

static BSTNode *min_node(BSTNode *node) {
    while (node->left) node = node->left;
    return node;
}

/* busca por id em toda a arvore (id nao e a chave da BST) */
static BSTNode *remove_by_id_rec(BSTNode *node, int id, int *count, int *found) {
    if (!node) return NULL;
    node->left  = remove_by_id_rec(node->left,  id, count, found);
    node->right = remove_by_id_rec(node->right, id, count, found);

    if (!(*found) && shape_get_id(node->data) == id) {
        *found = 1;
        (*count)--;
        if (!node->left && !node->right) {
            shape_destroy(node->data);
            free(node);
            return NULL;
        } else if (!node->left) {
            BSTNode *tmp = node->right;
            shape_destroy(node->data);
            free(node);
            return tmp;
        } else if (!node->right) {
            BSTNode *tmp = node->left;
            shape_destroy(node->data);
            free(node);
            return tmp;
        } else {
            /* dois filhos: substitui pelo sucessor in-order */
            BSTNode *succ = min_node(node->right);
            shape_destroy(node->data);
            node->data = succ->data;
            int dummy = 0;
            node->right = remove_by_id_rec(node->right,
                                            shape_get_id(succ->data),
                                            count, &dummy);
            (*count)++;
        }
    }
    return node;
}

static Shape find_by_id_rec(BSTNode *node, int id) {
    if (!node) return NULL;
    if (shape_get_id(node->data) == id) return node->data;
    Shape found = find_by_id_rec(node->left, id);
    if (found) return found;
    return find_by_id_rec(node->right, id);
}

BST bst_create(void) {
    BSTData *bd = (BSTData *)malloc(sizeof(BSTData));
    if (!bd) return NULL;
    bd->root  = NULL;
    bd->count = 0;
    return (BST)bd;
}

void bst_destroy(BST b) {
    assert(b != NULL);
    BSTData *bd = (BSTData *)b;
    destroy_rec(bd->root);
    free(bd);
}

void bst_insert(BST b, Shape s) {
    assert(b != NULL && s != NULL);
    BSTData *bd = (BSTData *)b;
    bd->root = insert_rec(bd->root, s, &bd->count);
}

void bst_remove_by_id(BST b, int id) {
    assert(b != NULL);
    BSTData *bd = (BSTData *)b;
    int found = 0;
    bd->root = remove_by_id_rec(bd->root, id, &bd->count, &found);
}

Shape bst_find_by_id(BST b, int id) {
    assert(b != NULL);
    return find_by_id_rec(((BSTData *)b)->root, id);
}

void bst_inorder(BST b, BSTVisitor fn, void *userdata) {
    assert(b != NULL && fn != NULL);
    inorder_rec(((BSTData *)b)->root, fn, userdata);
}

int bst_size(BST b) {
    assert(b != NULL);
    return ((BSTData *)b)->count;
}