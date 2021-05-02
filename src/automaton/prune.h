#pragma once

#include "automaton/automaton.h"

/**
 * @author Vlad Argatu
 * @date 26/04/2021
 * @param automaton: The automaton to prune recursively.
 * @param state_id: The id of the current state.
 * @param access: an array containing the access booleans.
 * @param espace: an array containign the espace boleans.

 This function should only be used for testing pruposes. 
 */
size_t _automaton_prune(Automaton * automaton, State * s, size_t * pref, int * escape, State ** higher, size_t * cpt);

/**
 * @author Vlad Argatu
 * @date 26/04/2021
 * @param automaton: The automaton to be pruned.
 */

 void automaton_prune(Automaton * automaton);
