#include <string.h>
#include "datatypes/matrix.h"
#include "utils/memory_utils.h"

Matrix *matrix_create(size_t height, size_t width)
{
    if (width == 0 || height == 0)
        return NULL;
    Matrix *mat = SAFEMALLOC(sizeof(Matrix));
    size_t len = height * width;
    mat->mat = SAFECALLOC(len, sizeof(LinkedList *));
    mat->height = height;
    mat->width = width;
    mat->y_capacity = 10;
    return mat;
}

void matrix_free(Matrix *mat)
{
    if(mat != NULL)
    {
        size_t len = mat->height * mat->width;
        for (size_t i = 0; i < len; i++)
            list_free(mat->mat[i]);
        free(mat->mat);
        free(mat);
    }
}

LinkedList *matrix_get(Matrix *mat, size_t x, size_t y)
{
    return mat->mat[mat->width * y + x];
}

void matrix_set(Matrix *mat, size_t x, size_t y, LinkedList *value)
{
    mat->mat[mat->width * y + x] = value;
}

void matrix_add_row(Matrix *mat)
{
    // If not enough memory expand
    if (mat->height >= mat->y_capacity)
    {
        mat->y_capacity *= MAT_GROWTH_FACTOR;
        mat->mat = SAFEREALLOC(mat->mat, sizeof(LinkedList *) *
                                   mat->width * mat->y_capacity);
    }

    for (size_t x = 0; x < mat->width; x++)
        matrix_set(mat, x, mat->height, NULL);
    mat->height++;
}
