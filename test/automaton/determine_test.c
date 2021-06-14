#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include "automaton/determine.h"
#include "utils.h"

Test(determine, a_end)
{
    Automaton *a_end =
        automaton_from_daut(TEST_PATH "automaton/determine_daut/a_end.daut", 3);
    Automaton *expected = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/a_end_determined.daut", 2);

    Automaton *determined = determine(a_end);
    ASSERT_AUTOMATON_EQ(expected, determined);

    automaton_free(a_end);
    automaton_free(determined);
    automaton_free(expected);
}

Test(determine, maybe_a_b_star_a)
{
    Automaton *aut =
        automaton_from_daut(TEST_PATH "automaton/determine_daut/a?b*a.daut", 3);
    Automaton *expected = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/a?b*a_determined.daut", 2);

    Automaton *determined = determine(aut);
    ASSERT_AUTOMATON_EQ(expected, determined);

    automaton_free(aut);
    automaton_free(determined);
    automaton_free(expected);
}

Test(determine, a_b_or_c_a)
{
    Automaton *aut = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/((a(b|c))a).daut", 6);
    Automaton *expected = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/((a(b|c))a)_determined.daut", 5);

    Automaton *determined = determine(aut);
    ASSERT_AUTOMATON_EQ(expected, determined);

    automaton_free(aut);
    automaton_free(determined);
    automaton_free(expected);
}
