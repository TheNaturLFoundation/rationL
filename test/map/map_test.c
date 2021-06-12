#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "datatypes/map.h"

static uint64_t stupid_hash(const void *x)
{
    (void)(x);  // Suppress the unused warning
    return 1;
}

Test(map, map_init)
{
    Map *map = Map(int, int, NULL, NULL);

    cr_assert_eq(map->buckets->size, MAP_INITIAL_CAP);
    cr_assert_eq(map->load_factor, MAP_LOAD_FACTOR);
    cr_assert_eq(map->size, 0);

    map_free(map);
}

Test(map, map_get_set)
{
    Map *map = Map(char *, int, &hash_string, &compare_strings);

    char *key = "10";
    int value = 10;

    map_set(map, &key, &value);
    int *actual = map_get(map, &key);
    cr_assert_eq(*actual, value);

    cr_assert_eq(map->size, 1);

    map_free(map);
}

Test(map, map_get_not)
{
    Map *map = Map(char *, int, &hash_string, &compare_strings);

    char *key = "10";
    cr_assert_eq(map_get(map, &key), NULL);

    map_free(map);
}

Test(map, map_set_multiple)
{
    Map *map = Map(char *, int, &hash_string, &compare_strings);

    char *first_key = "a";
    char *second_key = "b";
    int first_value = 10;
    int second_value = 12;

    map_set(map, &first_key, &first_value);
    map_set(map, &second_key, &second_value);

    int *first = map_get(map, &first_key);
    int *second = map_get(map, &second_key);
    cr_assert_eq(*first, first_value);
    cr_assert_eq(*second, second_value);

    cr_assert_eq(map->size, 2);

    map_free(map);
}

Test(map, map_collision)
{
    Map *map = Map(char *, int, &stupid_hash, &compare_strings);

    char *first_key = "a";
    char *second_key = "b";
    int value = 10;

    map_set(map, &first_key, &value);
    map_set(map, &second_key, &value);

    int *first = map_get(map, &first_key);
    int *second = map_get(map, &second_key);
    cr_assert_eq(*first, value);
    cr_assert_eq(*second, value);

    map_free(map);
}

Test(map, map_update)
{
    Map *map = Map(char *, int, &hash_string, &compare_strings);

    char *key = "10";
    int value = 10;
    map_set(map, &key, &value);
    value = 42;
    map_set(map, &key, &value);

    int *actual = map_get(map, &key);
    cr_assert_eq(*actual, value);

    cr_assert_eq(map->size, 1);

    map_free(map);
}

Test(map, map_grow)
{
    Map *map = Map(int, int, &hash_int, &compare_ints);

    size_t i;
    for (i = 0; i < MAP_INITIAL_CAP * MAP_LOAD_FACTOR; i++)
        map_set(map, &i, &i);
    cr_assert_eq(map->size, MAP_INITIAL_CAP * MAP_LOAD_FACTOR);
    cr_assert_eq(map->buckets->size, MAP_INITIAL_CAP);

    map_set(map, &i, &i);  // BOOM! Rehash!
    cr_assert_eq(map->size, MAP_INITIAL_CAP * MAP_LOAD_FACTOR + 1);
    cr_assert_eq(map->buckets->size, MAP_INITIAL_CAP * MAP_GROWTH_FACTOR);

    for (i = 0; i <= MAP_INITIAL_CAP * MAP_LOAD_FACTOR; i++)
        cr_assert_eq(i, *(int *) map_get(map, &i));

    map_free(map);
}

Test(map, map_union)
{
    Map *map1 = Map(int, int, &hash_int, &compare_ints);
    Map *map2 = Map(int, int, &hash_int, &compare_ints);

    for (int i = 0; i < 3; i++)
    {
        int x = i + 1;
        map_set(map1, &i, &x);
    }
    for (int i = 2; i < 5; i++)
    {
        int x = i * 2;
        map_set(map2, &i, &x);
    }

    map_union(map1, map2);

    cr_assert_eq(map1->size, 5);
    for (int i = 0; i < 2; i++)
        cr_assert_eq(*(int *)map_get(map1, &i), i + 1);
    for (int i = 2; i < 5; i++)
        cr_assert_eq(*(int *)map_get(map1, &i), i * 2);

    cr_assert_eq(map2->size, 3);
    for (int i = 2; i < 5; i++)
        cr_assert_eq(*(int *)map_get(map2, &i), i * 2);

    map_free(map1);
    map_free(map2);
}

Test(map, map_union_expand)
{
    Map *map1 = Map(int, int, &hash_int, &compare_ints);
    Map *map2 = Map(int, int, &hash_int, &compare_ints);

    int i;
    for (i = 0; i < MAP_INITIAL_CAP * MAP_LOAD_FACTOR; i++)
    {
        int x = i + 1;
        map_set(map1, &i, &x);
    }
    map_set(map2, &i, &i);

    map_union(map1, map2);
    cr_assert_eq(map1->size, MAP_INITIAL_CAP * MAP_LOAD_FACTOR + 1);
    cr_assert_eq(map1->buckets->size, MAP_INITIAL_CAP * MAP_GROWTH_FACTOR);

    for (i = 0; i < MAP_INITIAL_CAP * MAP_LOAD_FACTOR; i++)
        cr_assert_eq(*(int *)map_get(map1, &i), i + 1);
    cr_assert_eq(*(int *)map_get(map1, &i), i);

    map_free(map1);
    map_free(map2);
}

Test(map, map_delete_basic)
{
    Map * map1 = Map(int, int, hash_int, compare_ints);
    int x = 42;
    int k = 69;

    map_set(map1, &k, &x);
    
    int * r = map_delete(map1, &k);

    cr_assert_eq(map1->size, 0);
    cr_assert_eq(*r, x);

    free(r);

    map_free(map1);
}

Test(map, map_delete_not_present)
{
    Map * map1 = Map(int, int, hash_int, compare_ints);
    int x = 42;
    int k = 69;

    map_set(map1, &k, &x);
    
    k = 88;
    int * r = map_delete(map1, &k);

    cr_assert_eq(map1->size, 1);
    cr_assert_eq(r, NULL);

    map_free(map1);
}
