#pragma once

#include <stdlib.h>
#include "datatypes/array.h"
#include "datatypes/linked_list.h"

#define MAP_INITIAL_CAP 16
#define MAP_GROWTH_FACTOR 2.0
#define MAP_LOAD_FACTOR 0.75

#define Map(K, V, hash, compare) map_init(sizeof(K), sizeof(V), hash, compare)


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
 * Returns a pointer to the element associated to the key in the hash map.
 * @param map The map in which to look for the element.
 * @param key A pointer to the key.
 * @return If the key is present in the map, a pointer to the associated value,
 *         else NULL.
 */
void *map_get(Map *map, void *key);

/**
 * Associate a value to a key in a hash map.
 * If the key is already in the map, overwrite the value, else add it.
 * If the load factor exceeds the threshold, expand the map.
 * @param map The map in which to associate the value.
 * @param key A pointer to the key.
 * @param value A pointer to the value.
 */
void map_set(Map *map, void *key, void *value);

/**
 * Create the union of two maps, put the result in the first one.
 * When there is a conflict, the value is overwritten by the value in the source.
 * If the load factor exceeds the threshold, expand the map.
 * @param dst The map that wil be modified.
 * @param src The source map.
 */
void map_union(Map *dst, Map *src);

void map_free(Map *map);

// Hash and comparison functions

int compare_ints(const void *lhs, const void *rhs);
uint64_t hash_int(const void *key);

int compare_strings(const void *lhs, const void *rhs);
uint64_t hash_string(const void *key);
