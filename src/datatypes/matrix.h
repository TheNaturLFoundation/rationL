#ifndef __MATRIX_H_
#define __MATRIX_H_

#define MAT_GROWTH_FACTOR 2.0

#include <stdlib.h>
#include "datatypes/linked_list.h"

#define Matrix(height, width) \
    matrix_create(height, width)

typedef struct Matrix
{
    LinkedList **mat;
    size_t height;
    size_t width;

    /**
     * There may be more allocated memory than there really needs to be.
     * This value keeps track of the actual number of allocated rows.
     */
    size_t y_capacity;
} Matrix;

Matrix *matrix_create(size_t height, size_t width);

void matrix_free(Matrix *mat);

LinkedList *matrix_get(Matrix *mat, size_t x, size_t y);

void matrix_set(Matrix *mat, size_t x, size_t y, LinkedList* list);

void matrix_add_row(Matrix *mat);

#endif // __MATRIX_H_
