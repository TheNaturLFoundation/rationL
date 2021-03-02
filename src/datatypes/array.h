#pragma once
#include <stdlib.h>

#define ARR_BASE_CAPACITY 4
#define ARR_GROWTH_FACTOR 2.0

#define Array(type) array_create(sizeof(type))
#define arr_foreach(T, var, array) \
    T var; \
    for (size_t i = 0; i < (array)->size && \
         (((var) = *(T *)array_get(array, i)) || 1); i++)

/**
 * @struct Array
 * @brief Basic generic expandable array.
 * The initial capacity is defined by define ARR_BASE_CAPACITY.
 * When the array is full, the capacity is multiplied by define ARR_GROWTH_FACTOR.
 * @author Rostan Tabet
 * @date 01/03/2020
 */
typedef struct Array {
    size_t capacity;
    size_t size;
    size_t data_size;
    void *data;
} Array;


/**
 * Allocates memory for a new array with an
 * initial capacity of ARR_BASE_CAPACITY and a size of 0.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param data_size: The size of the data stored in the array.
 * @return A pointer to a new Array.
 */
Array *array_create(size_t data_size);

/**
 * Initializes and fills an array of size `size` with `value`.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param n: Number of elements.
 * @param value: Value to fill the array with.
 * @param data_size: The size of the data stored in the array.
 * @return A pointer to a new Array.
 */
Array *array_init(size_t n, const void *value, size_t data_size);

/**
 * Gets the element at the specified index
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param index: Position of the element.
 * @return a void* pointer pointing to the data.
 */
void *array_get(Array *array, size_t index);

/**
 * Sets a value in the array at the specified index.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param index: Position of the element.
 * @param value: New value for the element.
 */
void array_set(Array *array, size_t index, const void *value);

/**
 * Adds an element at the end of the array. The size is incremented.
 * Increases the capacity if necessary. The growth factor is defined
 * by ARR_GROWTH_FACTOR.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param value: The value to append.
 */
void array_append(Array *array, const void *value);

/**
 * Removes an element from an array and returns it.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param index: Position of the element to remove.
 * @return the element that was at the specified index.
 */
void *array_remove(Array *array, size_t index);

/**
 * Insert an element in an array.
 * @author Rostan Tabet
 * @date 01/03/2021
 * @param index: Position to insert the element, with 0 <= index <= size.
 * @param value: Element to insert.
 * @return the element that was at the specified index.
 */
void array_insert(Array *array, size_t index, void *value);

/**
 * Clears an array. The size is set to 0 but the capacity doesn't change.
 * Memory is NOT freed.
 * @author Rostan Tabet
 * @date 01/03/2021
 */
void array_clear(Array *array);

/**
 * Frees an array and all its elements, making it impossible to use.
 * @author Rostan Tabet
 * @date 01/03/2021
 */
void array_free(Array *array);

/**
 * Applies a function to all elements in an array.
 * @author Rostan Tabet
 * @date 02/03/2021
 * @param function: The function to apply to the array.
 */
void array_map(Array *array, void (*function)(void *));

/**
 * Applies a function to all elements in an array and their indices.
 * @author Rostan Tabet
 * @date 02/03/2021
 * @param function: The function to apply to the array.
 */
void array_mapi(Array *array, void (*function)(size_t, void *));
