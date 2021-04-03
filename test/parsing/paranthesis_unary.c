#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(paranthesis_unary, left_a_right_star)
{
    Array *arr = tokenise("(a)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, left_a_right_star_b)
{
    Array *arr = tokenise("(a)*b");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_right_star_b.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, left_a_right_star_or_b)
{
    Array *arr = tokenise("(a)*|b");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_right_star_or_b.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, a_left_b_right_star)
{
    Array *arr = tokenise("a(b)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_left_b_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, a_or_left_b_right_star)
{
    Array *arr = tokenise("a|(b)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_b_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, left_ab_right_star)
{
    Array *arr = tokenise("(ab)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_ab_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, left_a_or_b_right_star)
{
    Array *arr = tokenise("(a|b)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_or_b_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, a_left_bc_right_star)
{
    Array *arr = tokenise("a(bc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_left_bc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary, a_or_left_bc_right_star)
{
    Array *arr = tokenise("a|(bc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_bc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}
