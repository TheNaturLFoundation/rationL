#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(unary_basics, a_star)
{
    Array *arr = tokenise("a*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(unary_basics, ab_star)
{
    Array *arr = tokenise("ab*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/ab_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(unary_basics, a_star_b)
{
    Array *arr = tokenise("a*b");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_star_b.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(unary_basics, a_or_b_star)
{
    Array *arr = tokenise("a|b*");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_or_b_star.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}

Test(unary_basics, a_star_or_b)
{
    Array *arr = tokenise("a*|b");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    BinTree *expected = bin_tree_from_dot("expected_trees/a_star_or_b.dot");
    cr_assert_true(bintree_compare(expected, got));
    bintree_free(got);
}
