#include "matrix.h"

#include <string.h>

#include "utils/memory_utils.h"

Matrix *matrix_create(size_t height, size_t width, long default_value)
{
    if (width == 0 || height == 0)
        return NULL;
    Matrix *mat = SAFEMALLOC(sizeof(Matrix));
    if (default_value == 0)
    {
        mat->mat = SAFECALLOC(height * width, sizeof(long));
    }
    else
    {
        mat->mat = SAFEMALLOC(height * width * sizeof(long));
        memset(mat->mat, default_value, height * width * sizeof(long));
    }
    mat->height = height;
    mat->width = width;
    return mat;
}

void matrix_free(Matrix *mat)
{
    free(mat->mat);
    free(mat);
}

long matrix_get(Matrix *mat, size_t x, size_t y)
{
    return mat->mat[mat->width * y + x];
}

void matrix_set(Matrix *mat, size_t x, size_t y, long value)
{
    mat->mat[mat->width * y + x] = value;
}
