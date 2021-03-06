#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"

Test(binary_basics, null)
{
    BinTree *got = parse_symbols(NULL);
    cr_assert_eq(got, NULL);
}

Test(binary_basics, a, .disabled = 1)
{
    // Disabled because for a simple string
    // a DFA is directly constructed
    Array *arr = tokenize("a");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a.dot");
    array_free(arr);
}

Test(binary_basics, ab, .disabled = 1)
{
    // Disabled because for a simple string
    // a DFA is directly constructed
    Array *arr = tokenize("ab");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/ab.dot");
    array_free(arr);
}

Test(binary_basics, a_or_b)
{
    Array *arr = tokenize("a|b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_b.dot");
    array_free(arr);
}

Test(binary_basics, abc, .disabled = 1)
{
    // Disabled because for a simple string
    // a DFA is directly constructed
    Array *arr = tokenize("abc");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/abc.dot");
    array_free(arr);
}

Test(binary_basics, a_or_b_or_c)
{
    Array *arr = tokenize("a|b|c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_b_or_c.dot");
    array_free(arr);
}

Test(binary_basics, ab_or_c)
{
    Array *arr = tokenize("ab|c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/ab_or_c.dot");
    array_free(arr);
}

Test(binary_basics, a_or_bc)
{
    Array *arr = tokenize("a|bc");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_bc.dot");
    array_free(arr);
}
