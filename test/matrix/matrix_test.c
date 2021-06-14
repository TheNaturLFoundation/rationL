#include <criterion/criterion.h>
#include "datatypes/matrix.h"

Test(matrix, matrix_init_0_0)
{
    Matrix * mat = Matrix(0, 0);
    cr_assert_eq(mat, NULL);
}

Test(matrix, matrix_init_h_0)
{
    Matrix * mat = Matrix(10, 0);
    cr_assert_eq(mat, NULL);
}

Test(matrix, matrix_init_h_w)
{
    Matrix * mat = Matrix(10, 5);
    cr_assert_eq(mat->height, 10);
    cr_assert_eq(mat->width, 5);
    matrix_free(mat);
}

Test(matrix, matrix_init_null)
{
    Matrix *mat = Matrix(10, 5);

    for (size_t i = 0; i < mat->width; i++)
        for (size_t j = 0; j < mat->height; j++)
            cr_assert_eq(NULL, matrix_get(mat, i, j));

    matrix_free(mat);
}

Test(matrix, matrix_get)
{
    Matrix *mat = Matrix(10, 5);

    LinkedList *list = LinkedList(int);
    // Put the list at the position (2, 3)
    mat->mat[5 * 3 + 2] = list;
    cr_assert_neq(NULL, matrix_get(mat, 2, 3));

    matrix_free(mat);
}

Test(matrix, matrix_set)
{
    Matrix *mat = Matrix(10, 5);

    LinkedList *list = LinkedList(int);
    matrix_set(mat, 2, 3, list);
    cr_assert_neq(NULL, matrix_get(mat, 2, 3));

    matrix_free(mat);
}

Test(matrix, matrix_add_row)
{
    Matrix *mat = Matrix(10, 5);
    cr_assert_eq(10, mat->y_capacity);

    matrix_add_row(mat);
    cr_assert_eq(10 * MAT_GROWTH_FACTOR, mat->y_capacity);
    cr_assert_eq(11, mat->height);
    cr_assert_eq(5, mat->width);

    // Check that the values are all equal to NULL
    for (size_t i = 0; i < mat->width; i++)
        for (size_t j = 0; j < mat->height; j++)
            cr_assert_eq(NULL, matrix_get(mat, i, j));

    // If we add this we should keep the same capacity
    cr_assert_eq(mat->y_capacity, 10 * MAT_GROWTH_FACTOR);
    for (size_t i = 11; i < 10 * MAT_GROWTH_FACTOR; i++)
        matrix_add_row(mat);

    // Boom! Expand!
    matrix_add_row(mat);
    cr_assert_eq(mat->y_capacity, 10 * MAT_GROWTH_FACTOR * MAT_GROWTH_FACTOR);

    matrix_free(mat);
}
