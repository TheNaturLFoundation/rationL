#ifndef __MATRIX_H_
#define __MATRIX_H_

#include <stdlib.h>

#define Matrix(height, width, default_value)                                   \
    matrix_create(height, width, default_value)

typedef struct Matrix
{
    long *mat;
    size_t height;
    size_t width;
} Matrix;

Matrix *matrix_create(size_t height, size_t width, long default_value);

void matrix_free(Matrix *mat);

long matrix_get(Matrix *mat, size_t x, size_t y);

void matrix_set(Matrix *mat, size_t x, size_t y, long val);

#endif // __MATRIX_H_
