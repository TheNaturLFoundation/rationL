#pragma once
#include "automaton.h"

/**
 * Determine an automaton using the powerset construction
 * @param source The source NFA without epsilon-moves.
 * @return An equivalent DFA.
 */
Automaton *determine(const Automaton *source);
