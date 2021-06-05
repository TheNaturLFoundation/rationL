#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include "automaton/determine.h"
#include "utils.h"

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

Test(build_search_dfa, offset)
{
    Automaton *source =
        automaton_from_daut(TEST_PATH "automaton/abcdabd.daut", 8);
    Automaton *result = build_search_dfa(source);
    automaton_free(source);

    LinkedList *tr;

    tr = get_matrix_elt(result, 0, 'a', 0);
    cr_assert_eq(tr->data, (void *)0);

    tr = get_matrix_elt(result, 1, 'a', 0);
    cr_assert_eq(tr->data, (void *)1);

    tr = get_matrix_elt(result, 3, 'a', 0);
    cr_assert_eq(tr->data, (void *)1, "Expected 0x1, got %p", tr->data);

    tr = get_matrix_elt(result, 6, 'c', 0);
    cr_assert_eq(tr->data, (void *)3);

    automaton_free(result);
}
