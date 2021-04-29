#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"


Test(unary_basics, a_star)
{
    Array *arr = tokenize("a*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_star.dot");
    array_free(arr);
}

Test(unary_basics, ab_star)
{
    Array *arr = tokenize("ab*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/ab_star.dot");
    array_free(arr);
}

Test(unary_basics, a_star_b)
{
    Array *arr = tokenize("a*b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_star_b.dot");
    array_free(arr);
}

Test(unary_basics, a_or_b_star)
{
    Array *arr = tokenize("a|b*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_b_star.dot");
    array_free(arr);
}

Test(unary_basics, a_star_or_b)
{
    Array *arr = tokenize("a*|b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_star_or_b.dot");
    array_free(arr);
}
