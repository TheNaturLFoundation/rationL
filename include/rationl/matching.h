#pragma once

#include "automaton.h"

/**
 * Test if a string is recognized by a non deterministic automaton.
 * This function tests a string matching.
 * @author Rostan Tabet
 * @date 26/03/2021
 * @param automaton Some NFA.
 * @param string The string to test.
 * @return 1 if the string is recognized by the nfa, else 0.
 */
int match_nfa(const Automaton *automaton, const char *string);
