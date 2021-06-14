#pragma once
#include "automaton.h"

/**
 * Determine an automaton using the powerset construction
 * @param source The source NFA without epsilon-moves.
 * @return An equivalent DFA.
 */
Automaton *determine(const Automaton *source);

/**
 * Build a DFA with extra transitions for optimal
 * substring search without having to backtrack.
 * @param automaton The source automaton without extra transitions.
 * @return The DFA specialized for substring search.
 */
Automaton *build_search_dfa(Automaton *automaton);
