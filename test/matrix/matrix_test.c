#include <criterion/criterion.h>
#include "datatypes/matrix.h"

Test(matrix, matrix_init_0_0)
{
    Matrix * mat = Matrix(0, 0, 0);
    cr_assert_eq(mat, NULL);
}

Test(matrix, matrix_init_h_0)
{
    Matrix * mat = Matrix(10, 0, 0);
    cr_assert_eq(mat, NULL);
}

Test(matrix, matrix_init_h_w)
{
    Matrix * mat = Matrix(10, 5, 0);
    cr_assert_eq(mat->height, 10);
    cr_assert_eq(mat->width, 5);
}
