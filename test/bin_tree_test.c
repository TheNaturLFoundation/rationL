#include "datatypes/bin_tree.h"
#include <criterion/criterion.h>
#include <stdio.h>

Test(bin_tree, create_tree_single_node)
{
    BinTree * node = BinTree(int, 12);   
    cr_assert_eq(*((int *)node->data), 12);
    bintree_free(node);
}

Test(bin_tree, create_tree_left_node)
{
    BinTree *left = BinTree(int, 2);
    BinTree *parent = BinTree(int, 4, .left = left);
    cr_assert_eq(*(int *)parent->left->data, 2);
    cr_assert_eq(*(int *)parent->data, 4);
}
