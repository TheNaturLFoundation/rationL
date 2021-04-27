#include "map.h"

#include <printf.h>
#include <string.h>

#include "utils/memory_utils.h"

#define STR_HASH_MAGIC_NUMBER 5381
#define INT_HASH_MAGIC_NUMBER 0x45d9f3b

static void map_grow(Map *map);
static void _map_set(Map *map, void *key, void *value);

Map *map_init(size_t key_size, size_t val_size,
              uint64_t (*hash)(const void *),
              int (*compare)(const void *, const void *))
{
    Map *map = SAFEMALLOC(sizeof(Map));

    map->hash = hash;
    map->compare = compare;
    map->size = 0;
    map->key_size = key_size;
    map->val_size = val_size;
    map->load_factor = MAP_LOAD_FACTOR;
    map->growth_factor = MAP_GROWTH_FACTOR;
    map->buckets = Array(LinkedList *);
    for (size_t i = 0; i < MAP_INITIAL_CAP; i++)
    {
        LinkedList *list = LinkedList(MapNode *);
        array_append(map->buckets, &list);
    }

    return map;
}

void map_free(Map *map)
{
    arr_foreach(LinkedList *, list, map->buckets)
    {
        list_foreach(MapNode *, node, list)
        {
            free(node->key);
            free(node->value);
            free(node);
        }
        list_free(list);
    }
    array_free(map->buckets);
    free(map);
}

void *map_get(Map *map, void *key)
{
    size_t index = map->hash(key) % map->buckets->size;
    LinkedList *bucket = *(LinkedList **) array_get(map->buckets, index);

    list_foreach(MapNode *, node, bucket)
    {
        if (map->compare(node->key, key) == 0)
            return node->value;
    }

    return NULL;
}


void map_set(Map *map, void *key, void *value)
{
    _map_set(map, key, value);

    if ((float)map->size / map->buckets->size > map->load_factor)
        map_grow(map);
}

void map_union(Map *dst, Map *src)
{
    arr_foreach(LinkedList *, bucket, src->buckets)
    {
        list_foreach(MapNode *, node, bucket)
            map_set(dst, node->key, node->value);
    }
}


// Hash and comparison functions

int compare_ints(const void *lhs, const void *rhs)
{
    return *(int *)lhs - *(int *)rhs;
}

uint64_t hash_int(const void *key)
{
    int x = *(int *)key;

    // I trust the guy from Stack Overflow that
    // calculated the magic number for me.
    // The result should be "nearly as good (not quite) as when using AES".
    x = ((x >> 16) ^ x) * INT_HASH_MAGIC_NUMBER;
    x = ((x >> 16) ^ x) * INT_HASH_MAGIC_NUMBER;
    x = (x >> 16) ^ x;

    return x;
}

int compare_strings(const void *lhs, const void *rhs)
{
    char *first = *(char **)lhs;
    char *second = *(char **)rhs;

    while (*first != 0 && *second != 0 && *first != *second)
    {
        first++;
        second++;
    }

    return *first - *second;
}

uint64_t hash_string(const void *key)
{
    unsigned char *string = *(unsigned char **) key;

    size_t hash = STR_HASH_MAGIC_NUMBER;  // Magic number
    unsigned char c;

    while ((c = *string++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + x */

    return hash;
}

// Local functions

static void map_grow(Map *map)
{
    // Move the old buckets
    Array *buckets = Array(LinkedList *);
    for (size_t i = 0; i < map->buckets->size; i++)
    {
        LinkedList *bucket = LinkedList(MapNode *);
        LinkedList *old_bucket = *(LinkedList **)array_get(map->buckets, i);
        bucket->next = old_bucket->next;
        old_bucket->next = NULL;
        if (bucket->next != NULL)
            bucket->next->previous = bucket;

        array_append(buckets, &bucket);
    }

    // Expand the array
    size_t new_size = map->buckets->size * map->growth_factor;
    for (size_t i = map->buckets->size; i < new_size; i++)
    {
        LinkedList *bucket = LinkedList(MapNode *);
        array_append(map->buckets, &bucket);
    }

    // Re-hash and free the new bucket array
    map->size = 0;
    arr_foreach(LinkedList *, bucket, buckets)
    {
        list_foreach(MapNode *, node, bucket)
        {
            _map_set(map, node->key, node->value);
            free(node->key);
            free(node->value);
            free(node);
        }
        list_free(bucket);
    }
    array_free(buckets);
}

static void _map_set(Map *map, void *key, void *value)
{
    size_t index = map->hash(key) % map->buckets->size;
    LinkedList *bucket = *(LinkedList **) array_get(map->buckets, index);

    list_foreach(MapNode *, node, bucket)
    {
        if (map->compare(node->key, key) == 0)
        {
            memcpy(node->value, value, map->val_size);
            return;
        }
    }

    MapNode *new_node = SAFEMALLOC(sizeof(MapNode));
    new_node->key = SAFEMALLOC(map->key_size);
    new_node->value = SAFEMALLOC(map->val_size);
    memcpy(new_node->key, key, map->key_size);
    memcpy(new_node->value, value, map->val_size);
    list_push_front(bucket, &new_node);

    map->size++;
}
