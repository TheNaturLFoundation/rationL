#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(paranthesis_more, a_left_bcd_right)
{
    Array *arr = tokenise("a(bcd)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_left_bcd_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_more, a_or_left_bcd_right)
{
    Array *arr = tokenise("a|(bcd)");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_left_bcd_right.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_more, left_abc_right_d)
{
    Array *arr = tokenise("(abc)d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_abc_right_d.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_more, left_abc_right_or_d)
{
    Array *arr = tokenise("(abc)|d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_abc_right_or_d.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(paranthesis_more, left_a_or_bc_right_or_d)
{
    Array *arr = tokenise("(a|bc)|d");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/left_a_or_bc_right_or_d.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}
