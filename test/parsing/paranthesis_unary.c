#include <criterion/criterion.h>
#include "rationl/bin_tree.h"
#include "rationl/parsing.h"
#include "test_utils.h"

Test(paranthesis_unary, left_a_right_star)
{
    Array *arr = tokenize("(a)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, left_a_right_star_b)
{
    Array *arr = tokenize("(a)*b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_star_b.dot");
    array_free(arr);
}

Test(paranthesis_unary, left_a_right_star_or_b)
{
    Array *arr = tokenize("(a)*|b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_star_or_b.dot");
    array_free(arr);
}

Test(paranthesis_unary, a_left_b_right_star)
{
    Array *arr = tokenize("a(b)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_b_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, a_or_left_b_right_star)
{
    Array *arr = tokenize("a|(b)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_b_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, left_ab_right_star)
{
    Array *arr = tokenize("(ab)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, left_a_or_b_right_star)
{
    Array *arr = tokenize("(a|b)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_b_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, a_left_bc_right_star)
{
    Array *arr = tokenize("a(bc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_bc_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary, a_or_left_bc_right_star)
{
    Array *arr = tokenize("a|(bc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_bc_right_star.dot");
    array_free(arr);
}
