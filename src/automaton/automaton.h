#pragma once

#include <stddef.h>
#include "utils/memory_utils.h"
#include "datatypes/linked_list.h"
#include "datatypes/array.h"
#include "parsing/lexer.h"

#define Automaton() automaton_create();

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

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @return Pointer on the new automaton allocated on the heap
*/
Automaton * automaton_create();


/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton on which the operation is performed.
 * @param state: the state to be added.
 * @param is_entry: boolean that specifies wether or not the state need to be added to the etry list.
 * @brief adds the state 'state' to the automaton 'automaton'.
*/
void automaton_add_state(Automaton * automaton, State * state, int is_entry);

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton on which the operation is performed.
 * @param state: the state to be deleted.  
*/

void automaton_remove_state(Automaton * automaton, State * state);

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton on which the operation is performed.
 * @param src: the source state in the adj_list.
 * @param dst: the destiation state.
 * @param value: the value of the transition, this value is ignored if epsilon is set.
 * @param epsilon: boolean, if set the transition is epsilon and value is ignored.
 * @brief adds the transition specified in the automaton.
*/

void automaton_add_transition(Automatonn * automaton, 
    State * src, State * dst, Letter value, int epsilon);


/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton on which the operation is performed.
 * @param src: the source state of the transition.
 * @param dst: the destination state of the trasition.
*/
void automaton_remove_transition(Automaton * automaton, State * src, State * dst);

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton to free.
*/
void automaton_free(Automaton * automaton);