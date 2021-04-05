#pragma once

#include "automaton/automaton.h"

/**
 * Test if a string is fully recognized by a non deterministic automaton.
 * @author Rostan Tabet
 * @date 26/03/2021
 * @param automaton Some NFA.
 * @param string The string to test.
 * @return 1 if the string is recognized by the nfa, else 0.
 */
int match_nfa(const Automaton *automaton, const char *string);

/**
 * Return all substrings of a string recognized by a given NFA.
 * @author Rostan Tabet
 * @date 05/04/2021
 * @param automaton Some NFA.
 * @param string The string to extract substrings from.
 * @return An array containing all the matched substrings.
 */
Array *search_nfa(const Automaton *automaton, const char *string);

/**
 * Replace all substrings of a string recognized by a given NFA by another
 * string.
 * @author Rostan Tabet
 * @date 05/04/2021
 * @param automaton Some NFA.
 * @param string The input string.
 * @param replace The replacement string.
 * @return A new string allocated in the heap containing all substitutions.
 */
char *replace_nfa(const Automaton *automaton, const char *string,
                  const char *replace);
