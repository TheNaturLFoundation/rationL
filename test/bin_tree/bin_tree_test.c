#include <criterion/criterion.h>
#include <stdio.h>

#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

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

Test(bin_tree_compare, null)
{
    cr_assert_eq(bintree_compare(NULL, NULL), 1);
}


Test(bin_tree_compare, null_and_node)
{
    Symbol s;
    s.type = LETTER;
    s.value.letter = 'a';
    s.group = 0;
    BinTree *b = BinTree(Symbol, &s, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(b, NULL), 0);
    bintree_free(b);
}


Test(bin_tree_compare, node_and_null)
{
    Symbol s;
    s.type = LETTER;
    s.value.letter = 'a';
    s.group = 0;
    BinTree *b = BinTree(Symbol, &s, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(NULL, b), 0);
    bintree_free(b);
}

Test(bin_tree_compare, node_and_node_letter_eq)
{
    Symbol s_a, s_b;
    s_a.type = LETTER;
    s_a.value.letter = 'a';
    s_a.group = 0;
    s_b.type = LETTER;
    s_b.value.letter = 'a';
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 1);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, node_and_node_letter_neq)
{
    Symbol s_a, s_b;
    s_a.type = LETTER;
    s_a.value.letter = 'a';
    s_a.group = 0;
    s_b.type = LETTER;
    s_b.value.letter = 'b';
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 0);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, node_and_node_operator_eq)
{
    Symbol s_a, s_b;
    s_a.type = OPERATOR;
    s_a.value.operator = CONCATENATION;
    s_a.group = 0;
    s_b.type = OPERATOR;
    s_b.value.operator = CONCATENATION;
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 1);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, node_and_node_operator_neq)
{
    Symbol s_a, s_b;
    s_a.type = OPERATOR;
    s_a.value.operator = CONCATENATION;
    s_a.group = 0;
    s_b.type = OPERATOR;
    s_b.value.operator = UNION;
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 0);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, node_and_node_type_neq)
{
    Symbol s_a, s_b;
    s_a.type = LETTER;
    s_a.value.letter = 'a';
    s_a.group = 0;
    s_b.type = OPERATOR;
    s_b.value.operator = CONCATENATION;
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 0);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, children_eq)
{
    Symbol s_a, s_b;
    s_a.type = LETTER;
    s_a.value.letter = 'a';
    s_a.group = 0;
    s_b.type = LETTER;
    s_b.value.letter='a';
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 1);
    bintree_free(a);
    bintree_free(b);
}

Test(bin_tree_compare, children_neq)
{
    Symbol s_a, s_b;
    s_a.type = LETTER;
    s_a.value.letter = 'a';
    s_a.group = 0;
    s_b.type = LETTER;
    s_b.value.letter ='b';
    s_b.group = 0;
    BinTree *a = BinTree(Symbol, &s_a, .left = NULL, .right = NULL);
    BinTree *b = BinTree(Symbol, &s_b, .left = NULL, .right = NULL);
    cr_assert_eq(bintree_compare(a, b), 0);
    bintree_free(a);
    bintree_free(b);
}
