#include <criterion/criterion.h>
#include <stdio.h>

#include "datatypes/bin_tree.h"

Test(bin_tree, create_tree_single_node)
{
    BinTree *node = BinTreeAtomic(int, 12, .left = NULL, .right = NULL);
    cr_assert_eq(*((int *)node->data), 12);
    bintree_free(node);
}

Test(bin_tree, create_tree_left_node)
{
    BinTree *left = BinTreeAtomic(int, 2, .left = NULL, .right = NULL);
    BinTree *parent = BinTreeAtomic(int, 4, .left = left);
    cr_assert_eq(*(int *)parent->left->data, 2);
    cr_assert_eq(*(int *)parent->data, 4);
    bintree_free(parent);
}

Test(bin_tree, create_tree_right_node)
{
    BinTree *right = BinTreeAtomic(int, 2, .left = NULL, .right = NULL);
    BinTree *parent = BinTreeAtomic(int, 4, .left = NULL, .right = right);
    cr_assert_eq(*(int *)parent->right->data, 2);
    cr_assert_eq(*(int *)parent->data, 4);
    bintree_free(parent);
}

Test(bin_tree, create_tree_left_right_node)
{
    BinTree *right = BinTreeAtomic(int, 1, .left = NULL, .right = NULL);
    BinTree *left = BinTreeAtomic(int, 2, .left = NULL, .right = NULL);
    BinTree *parent = BinTreeAtomic(int, 0, .right = right, .left = left);
    cr_assert_eq(*(int *)parent->right->data, 1);
    cr_assert_eq(*(int *)parent->left->data, 2);
    cr_assert_eq(*(int *)parent->data, 0);
    bintree_free(parent);
}
