#include <criterion/criterion.h>
#include "datatypes/array.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
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

Test(paranthesis_more, left_a_right_or_left_b_right_left_c_right)
{
    Array *arr = tokenize("(a)|(b)(c)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_or_left_b_right_left_c_right.dot");
    array_free(arr);
}

Test(paranthesis_more, paranthesis_random)
{
    Array *arr = tokenize("((a)|(b|cd)(e))f|gh|i(j|klm|(n)|(p)(qrs))");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/paranthesis_random.dot");
    array_free(arr);
}

Test(paranthesis_more, a_or_b_star_c)
{
    Array *arr = tokenize("a|b*c");
    BinTree *b = parse_symbols(arr);
    bin_tree_to_dot(b, stdout);
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_b_star_c.dot");
    array_free(arr);
}
