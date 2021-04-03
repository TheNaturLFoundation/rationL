#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(paranthesis_nested, left_left_a_right_right)
{
    Array *arr = tokenise("((a))");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_left_a_right_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, left_left_ab_right_right)
{
    Array *arr = tokenise("((ab))");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_left_ab_right_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, left_left_a_right_b_right)
{
    Array *arr = tokenise("((a)b)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_left_a_right_b_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, left_left_a_or_b_right_c_right_or_d)
{
    Array *arr = tokenise("((a|b)c)|d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_left_a_or_b_right_c_right_or_d.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, a_or_left_left_b_or_c_right_d_right)
{
    Array *arr = tokenise("a|((b|c)d)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_left_b_or_c_right_d_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, left_left_a_star_right_or_b_right_c)
{
    Array *arr = tokenise("((a*)|b)|c");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_left_a_star_right_or_b_right_c.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, a_or_left_left_b_star_right_or_c_right)
{
    Array *arr = tokenise("a|((b*)|c)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_left_b_star_right_or_c_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, left_a_left_b_or_c_right_star_right_or_d_right)
{
    Array *arr = tokenise("(a(b|c)*)|d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_left_b_or_c_right_star_right_or_d_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_nested, a_or_left_c_or_d_right_star_right)
{
    Array *arr = tokenise("a|(b(c|d)*)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_c_or_d_right_star_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}
