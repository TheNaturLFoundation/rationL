#pragma once
#include "utils/memory_utils.h"

/**
 * This value is used to be recognized while debugging which
 * of the linked list is the sentinel. It is affected to the data
 * attribute of the sentinel.
 */
#define SENTINEL_DATA 0x12345

/**
 * @struct LinkedList
 * @brief Basic single linked list.
 * @author Simon Scatton
 * @date 21/02/2020
 * @version 1.0.0
 */
typedef struct
{
    int data;
    struct LinkedList *next;
} LinkedList;