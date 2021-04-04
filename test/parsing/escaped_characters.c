#include <criterion/criterion.h>
#include "rationl/bin_tree.h"
#include "rationl/parsing.h"
#include "test_utils.h"

Test(escaped_characters, a_dot_b)
{
    Array *arr = tokenize("a\\.b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_dot_b.dot");
    array_free(arr);
}

Test(escaped_characters, a_pipe_b)
{
    Array *arr = tokenize("a\\|b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_pipe_b.dot");
    array_free(arr);
}

Test(escaped_characters, a_escaped_star)
{
    Array *arr = tokenize("a\\*");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_escaped_star.dot");
    array_free(arr);
}

Test(escaped_characters, a_escaped_exists)
{
    Array *arr = tokenize("a\\+");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_escaped_exists.dot");
    array_free(arr);
}

Test(escaped_characters, a_escaped_maybe)
{
    Array *arr = tokenize("a\\?");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_escaped_maybe.dot");
    array_free(arr);
}
