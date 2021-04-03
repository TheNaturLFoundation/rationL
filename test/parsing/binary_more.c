#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(binary_more, ab_or_cd)
{
    Array *arr = tokenize("ab|cd");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/ab_or_cd.dot");
    array_free(arr);
}

Test(binary_more, a_or_bc_or_d)
{
    Array *arr = tokenize("a|bc|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_bc_or_d.dot");
    array_free(arr);
}

Test(binary_more, a_or_bcd)
{
    Array *arr = tokenize("a|bcd");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_bcd.dot");
    array_free(arr);
}

Test(binary_more, ab_or_c_or_d)
{
    Array *arr = tokenize("ab|c|d");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/ab_or_c_or_d.dot");
    array_free(arr);
}
