#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(binary_more, a_left_abc_right)
{
    Array *arr = tokenize("a(bcd)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_bcd_right.dot");
    array_free(arr);
}

Test(binary_more, a_or_left_abc_right)
{
    Array *arr = tokenize("a|(bcd)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_bcd_right.dot");
    array_free(arr);
}

Test(binary_more, left_abc_right_d)
{
    Array *arr = tokenize("(abc)d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_d.dot");
    array_free(arr);
}

Test(binary_more, left_abc_right_or_d)
{
    Array *arr = tokenize("(abc)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right_or_d.dot");
    array_free(arr);
}

Test(binary_more, left_a_or_bc_right_or_d)
{
    Array *arr = tokenize("(a|bc)|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_bc_right_or_d.dot");
    array_free(arr);
}
