#pragma once

#include "datatypes/array.h"
#include "datatypes/matrix.h"
#include "parsing/lexer.h"

#define NUMBER_OF_SYMB 257
#define EPSILON_INDEX 256
#define Automaton(size, letter_count) automaton_create(size, letter_count);
#define State(term) state_create(term);

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
     * Transition table of the automaton.
     * Lines are states, columns are transitions.
     * Each cell contains a linked list of states.
     * The first column corresponds to epsilon transitions.
     *
     * Example:</br>
     *   | e | ... | A | B | C | ... |</br>
     * --|---|-----|---|---|---|-----|</br>
     * 0 | N | ... | 2 | 0 | 2 | ... |</br>
     * --|---|-----|---|---|---|-----|</br>
     * 1 | N | ... | 1 | 1 | 1 | ... |</br>
     * --|---|-----|---|---|---|-----|</br>
     * 2 | N | ... | 1 | 0 | 2 | ... |</br>
     * --|---|-----|---|---|---|-----|
     */
    Matrix *transition_table;

    /**
     * An array containing pointers on all of the entry points of the automaton.
     */
    Array *starting_states;

    /**
     * An array containing all states
    */
    Array *states;
	
    int * lookup_table;

    size_t lookup_used;

    int is_determined;

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
 * @author Vlad Argatu
 * @date 07/03/2021
 * @param is_terminal: booleen indicating if the state is terminal or not.
 * @return The heap allocated state.
*/

State * state_create(int is_terminal);

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @return Pointer on the new automaton allocated on the heap
*/
Automaton * automaton_create();


/**
 * @author Vlad Argatu
 * @data 21/04/2021
 * @param automatonn: the automaton.
 * @param state_id: the id of the state which is the source of the transition
 * @param value: the character of the transition.
 * @param is_epsilon: if set, the value is ignored and the transition is considered as epsilon.
 * @return  NULL if no transition with the character exists in the entire automaton, the return of matrix_get otherwise.
 */

LinkedList * get_matrix_elt(const Automaton * automaton, size_t state_id, Letter value, int is_epsilon);

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

void automaton_add_transition(Automaton * automaton, 
    State * src, State * dst, Letter value, int epsilon);


/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton on which the operation is performed.
 * @param src: the source state of the transition.
 * @param dst: the destination state of the trasition.
 * @param value: the value of the transition to remove.
 * @param epsilon: boolean, if set the value is ignored and it only deletes epsilon transitions between those states.
 * @return returns 0 if successfull 1 otherwise.

 If multiple transition have the same target with the same values, deletes the first one.
*/
int automaton_remove_transition(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon);

/**
 * @author Vlad Argatu
 * @date 05/03/2021
 * @param automaton: the automaton to free.
*/
void automaton_free(Automaton * automaton);

/**
 * @author Rostan Tabet
 * @date 24/03/2021
 * @filename: path to the .daut file.
 */
Automaton *automaton_from_daut(const char *filename);



/**
 * @author Simon Scatton
 * @date 27/03/2021
 * @param aut The automaton to build the dot of.
 * Prints the dot reprensentation of the automaton in the
 * stdout. */
void automaton_to_dot(Automaton* aut);
