#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
#include "test_utils.h"

Test(groups, left_ab_right_c_left_d_right)
{
    Array *arr = tokenize("(ab)c(d)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_ab_right_c_left_d_right.dot");
    array_free(arr);
}

Test(groups, left_a_right_left_b_right_left_c_right_left_d_right)
{
    Array *arr = tokenize("(a)(b)(c)(d)");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_a_right_left_b_right_left_c_right_left_d_right.dot");
    array_free(arr);
}

Test(groups, left_left_a_right_left_b_right_left_c_right_left_d_right_right)
{
    Array *arr = tokenize("((a)(b)(c)(d))");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_a_right_left_b_right_left_c_right_left_d_right_right.dot");
    array_free(arr);
}

Test(groups, left_left_ab_right_left_cd_right_left_e_right_right_left_f_right_g)
{
    Array *arr = tokenize("((ab)(cd)(e))(f)g");
    check_ast_from_file(arr, TEST_PATH "parsing/dotfiles/left_left_ab_right_left_cd_right_left_e_right_right_left_f_right_g.dot");
    array_free(arr);
}
