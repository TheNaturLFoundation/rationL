#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <criterion/internal/assert.h>
#include "datatypes/array.h"
#include <stdio.h>

Test(array, array_create)
{
    Array *array = Array(int);
    cr_assert_eq(array->capacity, ARR_BASE_CAPACITY);
    cr_assert_eq(array->size, 0);
    cr_assert_eq(array->data_size, sizeof(int));
    array_free(array);
}

Test(array, array_init)
{
    const size_t size = 10;
    const int value = 42;
    Array *array = array_init(size, &value, sizeof(int));

    int *p = array->data;
    for (size_t i = 0; i < size; i++, p++)
        cr_assert_eq(*p, value);
    array_free(array);
}

Test(array, array_get)
{
    const size_t size = 10;
    const int value = 42;
    Array *array = array_init(size, &value, sizeof(int));

    for (size_t i = 0; i < size; i++)
        cr_assert_eq(*(int *)array_get(array, i), value);
    array_free(array);
}

Test(array, array_get_error, .exit_code = 1)
{
    Array *array = Array(int);
    array_get(array, 10);
    cr_assert_stderr_eq_str("Index 10 is out of range for array of size 0", "");
    array_free(array);
}

Test(array, array_set)
{
    const int size = 10;
    const int value = 0;
    Array *array = array_init(size, &value, sizeof(int));

    for (int i = 0; i < size; i++)
    {
        array_set(array, i, &i);
        int *x = array_get(array, i);
        cr_assert_eq(*x, i);
    }
    array_free(array);
}

Test(array, array_set_error, .exit_code = 1)
{
    Array *array = Array(int);
    int x = 42;
    array_set(array, 10, &x);
    cr_assert_stderr_eq_str("Index 10 is out of range for array of size 0", "");
    array_free(array);
}

Test(array, array_append)
{
    Array *array = Array(int);

    int i;
    for (i = 0; i < ARR_BASE_CAPACITY; i++)
        array_append(array, &i);
    cr_assert_eq(array->capacity, ARR_BASE_CAPACITY);
    cr_assert_eq(array->size, ARR_BASE_CAPACITY);

    array_append(array, &i);
    cr_assert_eq(array->capacity, ARR_BASE_CAPACITY * ARR_GROWTH_FACTOR);
    cr_assert_eq(array->size, ARR_BASE_CAPACITY + 1);

    for (i = 0; i < array->size; i++)
        cr_assert_eq(*(int *)array_get(array, i), i);
    array_free(array);
}

Test(array, array_insert)
{
    const int value = -1;
    const size_t initial_size = ARR_BASE_CAPACITY;
    Array *array = array_init(initial_size, &value, sizeof(int));

    int x = 42;
    array_insert(array, array->size, &x);
    cr_assert_eq(array->size, initial_size + 1);

    x = 10;
    array_insert(array, 0, &x);
    cr_assert_eq(array->size, initial_size + 2);

    cr_assert_eq(*(int *)array_get(array, 0), 10);
    cr_assert_eq(*(int *)array_get(array, array->size - 1), 42);

    for (size_t i = 1; i < array->size - 1; i++)
        cr_assert_eq(*(int *)array_get(array, i), value);
    array_free(array);
}

Test(array, array_insert_error, .exit_code = 1)
{
    Array *array = Array(int);
    int x = 42;
    array_insert(array, 10, &x);
    cr_assert_stderr_eq_str("Index 10 is out of range for array of size 0", "");
    array_free(array);
}

Test(array, array_remove)
{
    const int value = -1;
    const size_t size = 10;
    Array *array = array_init(size, &value, sizeof(int));

    array_remove(array, 0);
    cr_assert_eq(array->size, size - 1);

    int x = 42;
    array_set(array, 2, &x);
    array_remove(array, 2);

    arr_foreach(int, curr, array)
        cr_assert_eq(curr, value);
    array_free(array);
}

Test(array, array_remove_error, .exit_code = 1)
{
    Array *array = Array(int);
    array_remove(array, 10);
    cr_assert_stderr_eq_str("Index 10 is out of range for array of size 0", "");
    array_free(array);
}

Test(array, array_clear)
{
    int x = 42;
    Array *array = array_init(10, &x, sizeof(int));
    size_t capacity = array->capacity;

    array_clear(array);
    cr_assert_eq(array->size, 0);
    cr_assert_eq(array->capacity, capacity);
    array_free(array);
}

Test(array, array_free)
{
    Array *array = Array(int);
    array_free(array);
}

static void square(void *a)
{
    int *x = a;
    *x *= *x;
}

Test(array, array_map)
{
    Array *array = Array(int);
    for (size_t i = 0; i < 10; i++)
        array_append(array, &i);

    array_map(array, square);
    for (size_t i = 0; i < array->size; i++)
        cr_assert_eq(*(int *)array_get(array, i), i * i);
    array_free(array);
}

static void square_and_add(size_t i, void *a)
{
    int *x = a;
    *x *= *x;
    *x += i;
}

Test(array, array_mapi)
{
    Array *array = Array(int);
    for (size_t i = 0; i < 10; i++)
        array_append(array, &i);

    array_mapi(array, square_and_add);
    for (size_t i = 0; i < array->size; i++)
        cr_assert_eq(*(int *)array_get(array, i), i * i + i);
    array_free(array);
}

Test(array, array_sub)
{
    Array *array = Array(int);
    for (size_t i = 0; i < 10; i++)
        array_append(array, &i);

    Array *sub = array_sub(array, 2, 5);
    array_free(array);

    cr_assert_eq(sub->size, 4);
    for (size_t i = 2; i <= 5; i++)
        cr_assert_eq(*(int *)array_get(sub, i - 2), i);

    array_free(sub);
}

Test(array, array_concat)
{
    Array *array = Array(int);
    for (size_t i = 0; i < 10; i++)
        array_append(array, &i);

    Array *array2 = Array(int);
    for (size_t i = 10; i < 20; i++)
        array_append(array2, &i);
    array_concat(array, array2);
    array_free(array2);

    cr_assert_eq(array->size, 20);
    for (size_t i = 0; i < array->size; i++)
        cr_assert_eq(*(int *)array_get(array, i), i);

    array_free(array);
}
