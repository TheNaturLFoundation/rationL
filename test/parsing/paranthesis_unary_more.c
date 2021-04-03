#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(paranthesis_unary_more, left_abc_right_star)
{
    Array *arr = tokenise("(abc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_abc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, a_left_bcd_right_star)
{
    Array *arr = tokenise("a(bcd)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_left_bcd_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, left_abc_right_star_d)
{
    Array *arr = tokenise("(abc)*d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_abc_right_star_d.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, left_ab_or_cd_right_star)
{
    Array *arr = tokenise("(ab|cd)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_ab_or_cd_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, left_a_or_bc_or_d_right_star)
{
    Array *arr = tokenise("(a|bc|d)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_or_bc_or_d_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, left_a_or_bc_right_star)
{
    Array *arr = tokenise("(a|bc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_or_bc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, a_star_left_bc_right_star)
{
    Array *arr = tokenise("a*(bc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_star_left_bc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_unary_more, left_ab_right_star_left_cd_right_star)
{
    Array *arr = tokenise("(ab)*(bc)*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_ab_right_star_left_bc_right_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}
