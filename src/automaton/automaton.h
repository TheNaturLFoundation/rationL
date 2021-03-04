#pragma once

#include <stddef.h>
#include "datatypes/linked_list.h"
#include "parsing/parsing.h"

/**
 * @struct Automaton
 * @brief Represents an automaton as a graph.
 * This struct is used to represent an automaton as a graph
 * using adjacency lists.
 * @author Simon Scatton
 * @date 21/02/2020
 * @version 1.0.0
 */
typedef struct
{
    /**
     * This order of the graph i.e. the number of states in the automaton.
     */
    size_t size;
    /**
     * The array `adj_lists` contains automaton transitions.
     * For the ith state, its transitions is the linked list adj_Lists[i]
     * where i is an integer from 0 to size - 1.
     * The size of the array is the order of the graph.
     */
    Array *adj_lists;
    /**
     * An array containing all of the entry points of the automaton.
     */
     Array *starting_states;
} Automaton;

/**
 * @struct State
 * @brief Representation of an automaton state.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct State
{
    /**
     * Identifier of the state in the automaton.
     * From 0 to n - 1 where n is the order of the graph.
     */
    size_t id;
    /**
     * True if the state is a terminal state.
     */
    int terminal;
} State;

/**
 * @struct Transition
 * @brief Representation of an automaton transition.
 * Contains the target state and the value of the transition itself.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct Transition
{
    State target;
    /**
     * Value of the transition if it is not an epsilon transition.
     * If the field `is_epsilon` is set to 1, the value can be anything.
     */
    Letter value;
    int is_epsilon;
} Transition;
