#include "utils.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/determine.h"

Test(build_search_dfa, abcdabd)
{
    Automaton *source =
        automaton_from_daut(TEST_PATH "automaton/abcdabd.daut", 8);
    Automaton *expected = automaton_from_daut(
        TEST_PATH "automaton/search_dfa_daut/abcdabd.daut", 8);

    Automaton *result = build_search_dfa(source);
    ASSERT_AUTOMATON_EQ(result, expected);

    automaton_free(result);
    automaton_free(source);
}

Test(build_search_dfa, a_or_bb_ab)
{
    Automaton *source =
    automaton_from_daut(TEST_PATH "automaton/(a|bb)ab.daut", 8);
    Automaton *expected = automaton_from_daut(
    TEST_PATH "automaton/search_dfa_daut/(a|bb)ab.daut", 8);

    Automaton *result = build_search_dfa(source);
    ASSERT_AUTOMATON_EQ(result, expected);

    automaton_free(result);
    automaton_free(source);
}
