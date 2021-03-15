#include "datatypes/bin_tree.h"
#include <criterion/criterion.h>
#include <stdio.h>

Test(bin_tree, create_tree_single_node)
{
    BinTree * node = BinTreeAtomic(int, 12);
    cr_assert_eq(*((int *)node->data), 12);
    bintree_free(node);
}

Test(bin_tree, create_tree_left_node)
{
    BinTree *left = BinTreeAtomic(int, 2);
    BinTree *parent = BinTreeAtomic(int, 4, .left = left);
    cr_assert_eq(*(int *)parent->left->data, 2);
    cr_assert_eq(*(int *)parent->data, 4);
    bintree_free(parent);
}

Test(bin_tree, create_tree_right_node)
{
    BinTree *right = BinTreeAtomic(int, 2);
    BinTree *parent = BinTreeAtomic(int, 4, .right = right);
    cr_assert_eq(*(int *)parent->right->data, 2);
    cr_assert_eq(*(int *)parent->data, 4);
    bintree_free(parent);
}

Test(bin_tree, create_tree_left_right_node)
{
    BinTree *right = BinTreeAtomic(int, 1);
    BinTree *left = BinTreeAtomic(int, 2);
    BinTree *parent = BinTreeAtomic(int, 0, .right = right, .left = left);
    cr_assert_eq(*(int *)parent->right->data, 1);
    cr_assert_eq(*(int *)parent->left->data, 2);
    cr_assert_eq(*(int *)parent->data, 0);
    bintree_free(parent);
}
