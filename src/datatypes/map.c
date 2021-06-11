#include "map.h"

#include <printf.h>
#include <string.h>

#include "utils/memory_utils.h"
#include "automaton/automaton.h"

#define STR_HASH_MAGIC_NUMBER 5381
#define INT_HASH_ADD_MAGIC_NUMBER 4355467
#define INT_HASH_MAGIC_NUMBER 0x45d9f3b


static void map_grow(Map *map);
static void _map_set(Map *map, const void *key, const void *value);

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

void map_clear(Map *map)
{
    arr_foreach(LinkedList *, bucket, map->buckets)
    {
        list_foreach(MapNode *, node, bucket)
        {
            free(node->key);
            free(node->value);
            free(node);
        }
        bucket->next = NULL;
    }
}

void map_free(Map *map)
{
    arr_foreach(LinkedList *, bucket, map->buckets)
    {
        list_foreach(MapNode *, node, bucket)
        {
            free(node->key);
            free(node->value);
            free(node);
        }
        list_free(bucket);
    }
    array_free(map->buckets);
    free(map);
}

void *map_get(const Map *map, const void *key)
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

void * map_delete(Map * map, const void * key)
{
    size_t index = map->hash(key) % map->buckets->size;
    LinkedList *bucket = *(LinkedList **) array_get(map->buckets, index);
    size_t list_index = 0;
    void *cpy;
    list_foreach(MapNode *, node, bucket)
    {
        if (map->compare(node->key, key) == 0)
        {
            cpy = node->value;
            free(node->key);
            list_free_from(list_pop_at(bucket, list_index));
            free(node);
            map->size--;
            return cpy;
        }
        list_index++;
    }

    return NULL;
}

void map_set(Map *map, const void *key, const void *value)
{
    _map_set(map, key, value);

    if ((float)map->size / map->buckets->size > map->load_factor)
        map_grow(map);
}

void map_union(Map *dst, const Map *src)
{
    arr_foreach(LinkedList *, bucket, src->buckets)
    {
        list_foreach(MapNode *, node, bucket)
            map_set(dst, node->key, node->value);
    }
}


// Hash and comparison functions

static uint64_t hash_number(size_t x)
{
    x += INT_HASH_ADD_MAGIC_NUMBER;

    // I trust the guy from Stack Overflow that
    // calculated the magic number for me.
    // The result should be "nearly as good (not quite) as when using AES".
    x = ((x >> 16) ^ x) * INT_HASH_MAGIC_NUMBER;
    x = ((x >> 16) ^ x) * INT_HASH_MAGIC_NUMBER;
    x = (x >> 16) ^ x;

    return x;
}

int compare_chars(const void *lhs, const void *rhs)
{
    return *(char *)lhs - *(char *)rhs;
}

int compare_uchars(const void *lhs, const void *rhs)
{
    unsigned char a = *(unsigned char *)lhs;
    unsigned char b = *(unsigned char *)rhs;

    return a > b ? 1 : a < b ? -1 : 0;
}

uint64_t hash_char(const void *key)
{
   char x = *(char *)key;
   return hash_number(x);
}

int compare_ints(const void *lhs, const void *rhs)
{
    return *(int *)lhs - *(int *)rhs;
}

uint64_t hash_int(const void *key)
{
    int x = *(int *)key;
    return hash_number(x);
}

int compare_size_t(const void *lhs, const void *rhs)
{
    size_t a = *(size_t *)lhs;
    size_t b = *(size_t *)rhs;

    return a > b ? 1 : a < b ? -1 : 0;
}

uint64_t hash_size_t(const void *key)
{
    size_t x = *(size_t *)key;
    return hash_number(x);
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
        hash = ((hash << 5) + hash) + c;

    return hash;
}

int compare_sets(const void *lhs, const void *rhs)
{
    const Map *map1 = *(Map **)lhs;
    const Map *map2 = *(Map **)rhs;

    if (map1->size != map2->size)
        return (int)((long)map1->size - (long)map2->size);

    arr_foreach(LinkedList *, bucket, map1->buckets)
    {
        list_foreach(MapNode *, node, bucket)
            if (!map_get(map2, node->key))
                return 1;
    }

    return 0;
}

uint64_t hash_set(const void *set)
{
    uint64_t hash = 0;

    const Map *map = *(Map **)set;
    arr_foreach(LinkedList *, bucket, map->buckets)
    {
        list_foreach(MapNode *, node, bucket)
            hash = ((hash << 5) + hash) + map->hash(node->key);
    }

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

static void _map_set(Map *map, const void *key, const void *value)
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

uint64_t hash_transition(const void *key)
{
    Transition * tr = *(Transition **)key;
    char * str_tr = transition_stringify(tr);
    uint64_t hash = hash_string(&str_tr);
    free(str_tr);
    return hash;
}

int compare_transitions(const void *lhs, const void *rhs)
{
    Transition * tr1 = *(Transition **)lhs;
    Transition * tr2 = *(Transition **)lhs;
    char * str_tr1 = transition_stringify(tr1);
    char * str_tr2 = transition_stringify(tr2);

    int result = compare_strings(&str_tr1, &str_tr2);

    free(str_tr1);
    free(str_tr2);

    return result;
}
