#pragma once

#include "automaton/automaton.h"

/**
 * Compute a regular expression associated to a given automaton.
 * @param automaton The input DFA.
 * @return A string representing the regular expression.
 * @warning This is the user's responsibility to free the string.
 */
char *stringify(Automaton *automaton);
