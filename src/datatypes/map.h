#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "datatypes/array.h"
#include "datatypes/linked_list.h"

#define MAP_INITIAL_CAP 16
#define MAP_GROWTH_FACTOR 2.0
#define MAP_LOAD_FACTOR 0.75

#define Map(K, V, hash, compare) map_init(sizeof(K), sizeof(V), hash, compare)
#define map_foreach_key(T, var, set, body)                                     \
    T var; \
    arr_foreach(LinkedList *, __##set##_bucket, (set)->buckets)                \
    {                                                                          \
        list_foreach(MapNode *, __##set##_node, __##set##_bucket)                \
        {                                                                      \
            var = *(T *)__##set##_node->key;                                  \
            body                                                               \
        }                                                                      \
    }

#define map_foreach_value(VT, var, map, body)                                     \
    VT var; \
    arr_foreach(LinkedList *, __##map##_bucket, (map)->buckets)                \
    {                                                                          \
        list_foreach(MapNode *, __##map##_node, __##map##_bucket)                \
        {                                                                      \
            var = *(VT *)__##map##_node->value;                                  \
            body                                                               \
        }                                                                      \
    }

/**
 * @struct MapNode
 * @brief A simple key-value pair used in a hash map
 */
typedef struct MapNode
{
    void *key;
    void *value;
} MapNode;

/**
 * @struct Map
 * @brief Implementation of a generic hash map
 * @author Rostan Tabet
 * @date 26/04/2021
 */
typedef struct Map
{
    /**
     * Each bucket is a linked list of nodes.
     * The array contains pointers to linked lists,
     * the linked lists contain nodes.
     * The capacity of the hash map is the size of the bucket array.
     */
    Array *buckets;

    /**
     * The number of elements in the hash map
     */
    size_t size;

    /**
     * The function used to hash the keys.
     * This function takes a pointer to the key as a parameter.
     */
    uint64_t (*hash)(const void *);

    /**
     * The function used to compare keys.
     * Must return an integer less than, equal to, or greater than zero if
     * the first argument is considered respectively less than, equal to, or
     * greater than the second.
     * Only equality is tested but well... you know, I can decide tomorrow to
     * use comparisons so you might as well respect the documentation.
     */
    int (*compare)(const void *, const void *);

    size_t key_size;

    size_t val_size;

    /**
     * The maximum load factor that determines when the map needs to be expanded
     */
    float load_factor;

    /**
     * The growth factor is the factor by which the capacity of the map is
     * expanded each time it needs to be.
     */
    float growth_factor;
} Map;

/**
 * Initializes an empty hash map with the default meta parameters.
 * @param key_size The size of the key type.
 * @param val_size The size of the value type.
 * @param hash The function used to hash the keys.
 * @return A pointer to the new hash map.
 */
Map *map_init(size_t key_size, size_t val_size,
              uint64_t (*hash)(const void *),
              int (*compare)(const void *, const void *));

/**
 * Clears and frees all the buckets.
 * @param map The map to clear.
 */
void map_clear(Map *map);


/**
 * Returns a pointer to the element associated to the key in the hash map.
 * @param map The map in which to look for the element.
 * @param key A pointer to the key.
 * @return If the key is present in the map, a pointer to the associated value,
 *         else NULL.
 */
void *map_get(const Map *map, const void *key);

/**
 * Returns a pointer to the element associated to the key in the hash map.
 * This element will be deleted from the hashmap, however the user
 * has to free the returned pointer because it is allocated by map_set.
 * @param map The map in which to look for the element.
 * @param key A pointer to the key.
 * @return If the key is present in the map, a pointer to the associated value,
 *         else NULL.
 */

void * map_delete(Map * map, const void * key);

/**
 * Associate a value to a key in a hash map.
 * If the key is already in the map, overwrite the value, else add it.
 * If the load factor exceeds the threshold, expand the map.
 * @param map The map in which to associate the value.
 * @param key A pointer to the key.
 * @param value A pointer to the value.
 */
void map_set(Map *map, const void *key, const void *value);

/**
 * Create the union of two maps, put the result in the first one.
 * When there is a conflict, the value is overwritten by the value in the source.
 * If the load factor exceeds the threshold, expand the map.
 * @param dst The map that wil be modified.
 * @param src The source map.
 */
void map_union(Map *dst, const Map *src);

void map_free(Map *map);

// Hash and comparison functions

int compare_uchars(const void *lhs, const void *rhs);
int compare_chars(const void *lhs, const void *rhs);
uint64_t hash_char(const void *key);

int compare_ints(const void *lhs, const void *rhs);
uint64_t hash_int(const void *key);

int compare_size_t(const void *lhs, const void *rhs);
uint64_t hash_size_t(const void *key);

int compare_strings(const void *lhs, const void *rhs);
uint64_t hash_string(const void *key);

uint64_t hash_transition(const void *key);
int compare_transitions(const void *lhs, const void *rhs);

/**
 * Compare two sets. If the first is less than, equal to, or greater than
 * the second, return a value respectively less than, equal to, or greater
 * than 0.
 * If the two sets have the same size but not the same elements,
 * the return value can be any non-zero integer.
 */
int compare_sets(const void *lhs, const void *rhs);
uint64_t hash_set(const void *set);

// A set is a map with no values

typedef Map Set;

#define Set(T, hash, compare) map_init(sizeof(T), 0, hash, compare)
#define set_add(set, key) map_set(set, key, NULL)

/**
 * Turns a set into a heap allocated string that wants to be free.
 * @param set: has to be a set of size_t, otherwise UB.
*/

char * stringify_set(Map * set, char pref);
