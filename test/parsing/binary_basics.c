#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

void check_ast_from_file(Array *arr, const char *path)
{
    BinTree *got = parse_symbols(arr);
    BinTree *expected = dot_to_bin_tree(path);
    cr_assert(bintree_compare(expected, got));
    bintree_free(got);
    bintree_free(expected);
}

Test(binary_basics, null)
{
    BinTree *got = parse_symbols(NULL);
    cr_assert_eq(got, NULL);
}

Test(binary_basics, a)
{
    Array *arr = tokenize("a");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a.dot");
    array_free(arr);
}


Test(binary_basics, ab)
{
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

Test(binary_basics, abc)
{
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
