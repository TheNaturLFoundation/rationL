#include <criterion/criterion.h>
#include "rationl/bin_tree.h"
#include "rationl/parsing.h"
#include "test_utils.h"

Test(paranthesis_more, a_left_abc_right)
{
    Array *arr = tokenize("a(bcd)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_bcd_right.dot");
    array_free(arr);
}

Test(paranthesis_more, a_or_left_abc_right)
{
    Array *arr = tokenize("a|(bcd)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_bcd_right.dot");
    array_free(arr);
}

Test(paranthesis_more, left_abc_right_d)
{
    Array *arr = tokenize("(abc)d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_d.dot");
    array_free(arr);
}

Test(paranthesis_more, left_abc_right_or_d)
{
    Array *arr = tokenize("(abc)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_or_d.dot");
    array_free(arr);
}

Test(paranthesis_more, left_a_or_bc_right_or_d)
{
    Array *arr = tokenize("(a|bc)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_bc_right_or_d.dot");
    array_free(arr);
}
