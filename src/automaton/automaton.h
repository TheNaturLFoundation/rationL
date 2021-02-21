#pragma once

#include <stddef.h>
#include "datatypes/linked_list.h"

/**
 * @struct Automaton
 * @brief Reprents an automaton as a graph.
 * This struct is used to represent an automaton as a graph
 * using adjacency lists.
 * @author Simon Scatton
 * @date 21/02/2020
 * @version 1.0.0
 */
typedef struct
{
    /**
     * This contains the number of states (nodes of the graph)
     */
    size_t state_count;
    /**
     * The adjacency list used to represent the graph. The length of
     * this adjacency list is state_count     
     */
    LinkedList *adj_list;
} Automaton;