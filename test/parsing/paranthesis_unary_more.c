#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"


Test(paranthesis_unary_more, left_abc_right_star)
{
    Array *arr = tokenize("(abc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, a_left_bcd_right_star)
{
    Array *arr = tokenize("a(bcd)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_bcd_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, left_abc_right_d_star)
{
    Array *arr = tokenize("(abc)*d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_star_d.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, left_ab_or_cd_right_star)
{
    Array *arr = tokenize("(ab|cd)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_or_cd_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, left_a_or_bc_or_d_right_star)
{
    Array *arr = tokenize("(a|bc|d)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_bc_or_d_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, left_a_or_bc_right_star)
{
    Array *arr = tokenize("(a|bc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_bc_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, a_start_right_bc_left_star)
{
    Array *arr = tokenize("a*(bc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_star_left_bc_right_star.dot");
    array_free(arr);
}

Test(paranthesis_unary_more, left_ab_right_star_left_bc_right_star)
{
    Array *arr = tokenize("(ab)*(bc)*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right_star_left_bc_right_star.dot");
    array_free(arr);
}
