#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"

Test(paranthesis_more, left_left_a_right_right)
{
    Array *arr = tokenize("((a))");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_a_right_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_left_ab_right_right)
{
    Array *arr = tokenize("((ab))");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_ab_right_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_left_a_right_b_right)
{
    Array *arr = tokenize("((a)b)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_a_right_b_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_left_a_or_b_right_c_right_or_d)
{
    Array *arr = tokenize("((a|b)c)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_a_or_b_right_c_right_or_d.dot");
    array_free(arr);
}

Test(paranthesis_more, a_or_left_left_b_or_c_right_d)
{
    Array *arr = tokenize("a|((b|c)d)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_left_b_or_c_right_d_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_left_a_star_right_or_b_right_or_c)
{
    Array *arr = tokenize("((a*)|b)|c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_a_star_right_or_b_right_or_c.dot");
    array_free(arr);
}

Test(paranthesis_more, a_or_left_left_b_star_right_or_c_right)
{
    Array *arr = tokenize("a|((b*)|c)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_left_b_star_right_or_c_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_a_left_b_or_c_right_star_right_or_d)
{
    Array *arr = tokenize("(a(b|c)*)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_left_b_or_c_right_star_right_or_d.dot");
    array_free(arr);
}

Test(paranthesis_more, a_or_left_b_left_c_or_d_right_star_right)
{
    Array *arr = tokenize("a|(b(c|d)*)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_b_left_c_or_d_right_star_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_a_or_left_bc_right_right_d)
{
    Array *arr = tokenize("(a|(bc))d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_left_bc_right_right_d.dot");
    array_free(arr);
}
