#include <string.h>
#include <stdio.h>
#include "utils/memory_utils.h"
#include "datatypes/array.h"

/**
 * Multiplies the capacity of the array by ARR_GROWTH_FACTOR.
 */
static void array_grow(Array *array)
{
    array->capacity *= ARR_GROWTH_FACTOR;
    array->data = SAFEREALLOC(array->data, array->capacity);
}

Array *array_create(size_t data_size)
{
    Array *array = SAFEMALLOC(sizeof(Array));
    array->capacity = ARR_BASE_CAPACITY;
    array->size = 0;
    array->data_size = data_size;
    array->data = SAFEMALLOC(data_size * ARR_BASE_CAPACITY);

    return array;
}

Array *array_init(size_t n, const void *value, size_t data_size)
{
    Array *array = array_create(data_size);
    array->size = n;
    array->data_size = data_size;
    size_t capacity = ARR_BASE_CAPACITY;
    while (capacity < n)
        capacity *= ARR_GROWTH_FACTOR;
    array->capacity = capacity;
    array->data = SAFEMALLOC(data_size * capacity);

    char *p = array->data;
    for (size_t i = 0; i < n; i++)
        memcpy(p + i * data_size, value, data_size);

    return array;
}

void *array_get(Array *array, size_t index)
{
    if (index >= array->size)
        errx(1, "Index %zu is out of range for array of size %zu\n",
             index, array->size);

    return (char *)array->data + index * array->data_size;
}

void array_set(Array *array, size_t index, const void *value)
{
    if (index >= array->size)
        errx(1, "Index %zu is out of range for array of size %zu\n",
             index, array->size);

    memcpy((char *)array->data + index * array->data_size,
           value, array->data_size);
}

void array_append(Array *array, const void *value)
{
    if (array->capacity <= array->size)
        array_grow(array);

    array->size++;
    array_set(array, array->size - 1, value);
}

void *array_remove(Array *array, size_t index)
{
    if (index >= array->size)
        errx(1, "Index %zu is out of range for array of size %zu\n",
             index, array->size);


    void *element = array_get(array, index);
    for (size_t i = index; i < array->size - 1; i++)
        array_set(array, i, array_get(array, i + 1));
    array->size--;

    return element;
}

void array_insert(Array *array, size_t index, void *value)
{
    if (index > array->size)
        errx(1, "Index %zu is out of range for array of size %zu\n",
             index, array->size);

    if (array->size >= array->capacity)
        array_grow(array);

    array->size++;
    for (size_t i = array->size - 1; i > index; i--)
        array_set(array, i, array_get(array, i - 1));

    array_set(array, index, value);
}

void array_clear(Array *array)
{
    array->size = 0;
}

void array_free(Array *array)
{
    array->size = 0;
    array->capacity = 0;
    free(array->data);
}

void array_map(Array *array, void (*function)(void *))
{
    for (size_t i = 0; i < array->size; i++)
        function(array_get(array, i));
}

void array_mapi(Array *array, void (*function)(size_t, void *))
{
    for (size_t i = 0; i < array->size; i++)
        function(i, array_get(array, i));
}
