#ifndef BST_H
#define BST_H

#include "shape.h"

/*
 * bst.h - Interface publica da arvore binaria de busca (BST).
 *
 * A BST armazena Shapes ordenados pela funcao shape_compare_default
 * (y crescente, depois x, depois area). Ela E DONA das shapes inseridas:
 * bst_destroy libera todos os nos e as respectivas shapes.
 *
 * Para valores iguais na chave de comparacao, o novo elemento vai para
 * a subarvore direita.
 */

typedef void *BST;

/* Funcao de visita usada no traversal em ordem. */
typedef void (*BSTVisitor)(Shape s, void *userdata);

/* Cria BST vazia. Retorna NULL em falha. */
BST bst_create(void);

/* Destroi a arvore liberando todos os nos e as shapes. */
void bst_destroy(BST b);

/* Insere shape na BST pela chave default. */
void bst_insert(BST b, Shape s);

/*
 * Remove o no cujo id corresponde ao parametro. Se nao encontrar,
 * nao faz nada. A shape removida e destruida.
 */
void bst_remove_by_id(BST b, int id);

/* Busca shape por id. Retorna NULL se nao encontrar. */
Shape bst_find_by_id(BST b, int id);

/* Traversal em ordem crescente (in-order). Chama fn para cada no. */
void bst_inorder(BST b, BSTVisitor fn, void *userdata);

/* Retorna o numero de nos na arvore. */
int bst_size(BST b);

#endif /* BST_H */