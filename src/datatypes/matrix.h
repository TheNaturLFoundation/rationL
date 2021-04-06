#ifndef __MATRIX_H_
#define __MATRIX_H_

#include <stdlib.h>
#include "datatypes/linked_list.h"

#define Matrix(height, width) \
    matrix_create(height, width)

typedef struct Matrix
{
    LinkedList **mat;
    size_t height;
    size_t width;
} Matrix;

Matrix *matrix_create(size_t height, size_t width);

void matrix_free(Matrix *mat);

LinkedList *matrix_get(Matrix *mat, size_t x, size_t y);

void matrix_set(Matrix *mat, size_t x, size_t y, LinkedList* list);

#endif // __MATRIX_H_
