#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"
/*
Test(l, l, .disabled=0)
{
    Array *arr = tokenize("(a|b)|c");
    BinTree *got = parse_symbols(arr);
    array_free(arr);
    FILE *file = fopen(TEST_PATH "parsing/dotfiles/left_a_or_b_right_or_c.dot", "w+");
    bin_tree_to_dot(got, file);
    fclose(file);
    bintree_free(got);
}*/

Test(binary_more, left_a_right)
{
    Array *arr = tokenize("(a)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right.dot");
    array_free(arr);
}

Test(binary_more, a_left_b_right)
{
    Array *arr = tokenize("a(b)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_b_right.dot");
    array_free(arr);
}

Test(binary_more, left_a_right_b)
{
    Array *arr = tokenize("(a)b");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_b.dot");
    array_free(arr);
}

Test(binary_more, left_ab_right)
{
    Array *arr = tokenize("(ab)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right.dot");
    array_free(arr);
}

Test(binary_more, left_a_or_b_right)
{
    Array *arr = tokenize("(a|b)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_b_right.dot");
    array_free(arr);
}

Test(binary_more, left_abc_right)
{
    Array *arr = tokenize("(abc)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_abc_right.dot");
    array_free(arr);
}

Test(binary_more, left_a_or_b_or_c_right)
{
    Array *arr = tokenize("(a|b|c)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_b_or_c_right.dot");
    array_free(arr);
}

Test(binary_more, a_left_bc_right)
{
    Array *arr = tokenize("a(bc)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_bc_right.dot");
    array_free(arr);
}

Test(binary_more, a_left_b_or_c_right)
{
    Array *arr = tokenize("a(b|c)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_left_b_or_c_right.dot");
    array_free(arr);
}

Test(binary_more, a_or_left_bc_right)
{
    Array *arr = tokenize("a|(bc)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_bc_right.dot");
    array_free(arr);
}

Test(binary_more, a_or_left_b_or_c_right)
{
    Array *arr = tokenize("a|(b|c)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/a_or_left_b_or_c_right.dot");
    array_free(arr);
}

Test(binary_more, left_ab_right_c)
{
    Array *arr = tokenize("(ab)c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right_c.dot");
    array_free(arr);
}

Test(binary_more, left_a_or_b_right_c)
{
    Array *arr = tokenize("(a|b)c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_b_right_c.dot");
    array_free(arr);
}

Test(binary_more, left_ab_right_or_c)
{
    Array *arr = tokenize("(ab)|c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right_or_c.dot");
    array_free(arr);
}

Test(binary_more, left_a_or_b_right_or_c)
{
    Array *arr = tokenize("(a|b)|c");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_or_b_right_or_c.dot");
    array_free(arr);
}
