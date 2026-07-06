#ifndef BST_H
#define BST_H

#include "shape.h"

typedef void *BST;

typedef void (*BSTVisitor)(Shape s, void *userdata);

BST   bst_create(void);
void  bst_destroy(BST b);
void  bst_insert(BST b, Shape s);
void  bst_remove_by_id(BST b, int id);
Shape bst_find_by_id(BST b, int id);
void  bst_inorder(BST b, BSTVisitor fn, void *userdata);
int   bst_size(BST b);

#endif