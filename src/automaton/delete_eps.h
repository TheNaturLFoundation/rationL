#pragma once

#include "automaton/automaton.h"

/**
 * @author Vlad Argatu
 * @date 24/04/2021
 * @param automaton: The automaton on which the operation is going to be performed.
 * @param src: The state from which the transitions will be copied.
 * @param new_src: The state that will recieve the transitions.
 */

void transfer_all_transitions(Automaton * automaton, State * src, State * new_src);

/**
 * @author Vlad Argatu
 * @date 24/04/2021
 * @param automaton: The automaton on which the operation is going to be performed.
 */

void automaton_delete_epsilon_tr(Automaton * automaton);
