#include "datatypes/matrix.h"

#include <string.h>

#include "automaton/automaton.h"
#include "utils/memory_utils.h"

Matrix *matrix_create(size_t height, size_t width)
{
    if (width == 0 || height == 0)
        return NULL;
    Matrix *mat = SAFEMALLOC(sizeof(Matrix));
    size_t len = height * width;
    mat->mat = malloc(len * sizeof(LinkedList *));
    for (size_t i = 0; i < len; i++)
        mat->mat[i] = NULL;
    mat->height = height;
    mat->width = width;
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
