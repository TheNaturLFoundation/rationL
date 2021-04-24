#pragma once

#include "automaton/automaton.h"
#include "datatypes/array.h"

/**
 * @struct Match
 * Contains all the necessary informations about a match.
 * @author Rostan Tabet
 * @date 20/04/2021
 */
typedef struct Match
{
    /**
     * The original string passed to the matching function.
     * @warning Not reallocated in the heap
     */
    const char *string;
    /**
     * The start position of the match in the original string.
     */
    size_t start;
    /**
     * The length of the match.
     */
    size_t length;

    /**
     * The number of groups matched from the regular expression.
     */
    size_t nb_groups;
    /**
     * An array with the content of all groups.
     */
    char **groups;
} Match;

/**
 * Test if an NFA matches the start of a string.
 * @author Rostan Tabet
 * @date 26/03/2021
 * @param automaton Some NFA.
 * @param string The string to test.
 * @return A pointer to a `Match` struct if the string is recognized by the nfa,
 * else NULL.
 */
Match *match_nfa(const Automaton *automaton, const char *string);

/**
 * Return all matches in a string recognized by a given NFA.
 * @author Rostan Tabet
 * @date 05/04/2021
 * @param automaton Some NFA.
 * @param string The string to extract substrings from.
 * @return An array containing all the matches.
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

/**
 * Frees an allocated `Match` struct
 */
void free_match(Match *match);
