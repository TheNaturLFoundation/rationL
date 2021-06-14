#pragma once

#include "automaton/automaton.h"

/**
 * @author Vlad Argatu
 * @date 24/04/2021
 * @param automaton: The automaton on which the operation is going to be performed.
 * @param src: The state from which the transitions will be copied.
 * @param new_src: The state that will recieve the transitions.
 * @param eps_entering: The Set of entering transition in the epsilon.
 */

void transfer_all_transitions(Automaton * automaton, State * src, 
    State * new_src, Map * eps_entering, Array * pred_lists);

/**
 * @author Vlad Argatu
 * @date 24/04/2021
 * @param automaton: The automaton on which the operation is going to be performed.
 */

void automaton_delete_epsilon_tr(Automaton * automaton);

/*
    Some aux
*/
Array * build_pred_lists(Automaton * automaton);
void free_pred_lists(Array * pred_lists);
