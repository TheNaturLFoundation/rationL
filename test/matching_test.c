#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/automaton.h"
#include "matching/matching.h"

#define STRINGIZE_(x) #x
#define TEST_PATH STRINGIZE_(test/daut_files/)

Test(matching, abba)
{
    Automaton *abba = automaton_from_daut(TEST_PATH "abba.daut");
    cr_assert(match_nfa(abba, "abba"));
    cr_assert(!match_nfa(abba, "abb"));
    cr_assert(!match_nfa(abba, "abbaa"));
    cr_assert(!match_nfa(abba, ""));

    automaton_free(abba);
}

Test(matching, a_or_b)
{
    Automaton *a_or_b = automaton_from_daut(TEST_PATH "a+b.daut");
    cr_assert(match_nfa(a_or_b, "a"));
    cr_assert(match_nfa(a_or_b, "b"));
    cr_assert(!match_nfa(a_or_b, "c"));
    cr_assert(!match_nfa(a_or_b, "ab"));
    cr_assert(!match_nfa(a_or_b, "ba"));
    cr_assert(!match_nfa(a_or_b, ""));

    automaton_free(a_or_b);
}

Test(matching, ab_or_cstar)
{
    Automaton *ab_or_cstar = automaton_from_daut(TEST_PATH "ab+cstar.daut");
    cr_assert(match_nfa(ab_or_cstar, "c"));
    cr_assert(match_nfa(ab_or_cstar, "ab"));
    cr_assert(match_nfa(ab_or_cstar, ""));
    cr_assert(match_nfa(ab_or_cstar, "ccccccccccccc"));
    cr_assert(!match_nfa(ab_or_cstar, "ccccccccccccca"));
    cr_assert(!match_nfa(ab_or_cstar, "ac"));

    automaton_free(ab_or_cstar);
}
