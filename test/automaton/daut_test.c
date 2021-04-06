#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/automaton.h"

#define ASSERT_AUTOMATON_EQ(a1, a2) assert_automaton_eq(__LINE__, a1, a2)

void assert_automaton_eq(size_t line, Automaton *a1, Automaton *a2)
{
    // Test starting states
    cr_assert_eq(a1->starting_states->size, a2->starting_states->size,
                 "First automaton has %zu starting states, "
                 "second has %zu (line %zu)",
                 a1->starting_states->size, a2->starting_states->size, line);
    for (size_t i = 0; i < a1->starting_states->size; i++)
    {
        State *s1 = *(State **)array_get(a1->starting_states, i);
        int found = 0;
        arr_foreach(State *, s2, a2->starting_states)
            if (s2->id == s1->id)
            {
                found = 1;
                break;
            }
        cr_assert(found, "First automaton has state %zu as a starting state,"
                      " second hasn't (line %zu)", s1->id, line);
    }
    cr_assert_eq(a1->states->size, a2->states->size,
                 "First automaton has %zu states, second has %zu (line %zu)",
                 a1->states->size, a2->states->size, line);

    size_t terminal1 = 0;
    size_t terminal2 = 0;
    arr_foreach(State *, s1, a1->states)
        terminal1 += s1->terminal;
    arr_foreach(State *, s2, a2->states)
        terminal2 += s2->terminal;
    cr_assert_eq(terminal1, terminal2,
                 "First automaton has %zu terminal states, "
                 "second has %zu (line %zu)",
                 terminal1, terminal2, line);
    for (size_t i = 0; i < a1->states->size; i++)
    {
        State *s1 = *(State **)array_get(a1->states, i);
        if (!s1->terminal)
            continue;
        int found = 0;
        arr_foreach(State *, s2, a2->states)
            if (s2->terminal && s2->id == s1->id)
            {
                found = 1;
                break;
            }
        cr_assert(found, "First automaton has state %zu as a terminal state,"
                         " second hasn't (line %zu)", s1->id, line);
    }

    cr_assert_eq(a1->size, a2->size,
                 "First automaton has size %zu, second has size %zu (line %zu)",
                 a1->size, a2->size, line);

    cr_assert_eq(a1->transition_table->width, a2->transition_table->width,
                 "First table has width %zu, second has width %zu (line %zu)",
                 a1->transition_table->width, a2->transition_table->width, line);

    // Check if the two transition tables are equal
    for (size_t x = 0; x < a1->transition_table->width; x++)
        for (size_t y = 0; y < a1->size; y++)
        {
            LinkedList *list1 = matrix_get(a1->transition_table, x, y)->next;
            LinkedList *list2 = matrix_get(a2->transition_table, x, y)->next;
            while (list1 != NULL && list2 != NULL)
            {
                State *s1 = *(State **)list1->data;
                State *s2 = *(State **)list2->data;
                cr_assert_eq(s1->id, s2->id);
                cr_assert_eq(s1->terminal, s2->terminal);

                list1 = list1->next;
                list2 = list2->next;
            }
            cr_assert_eq(list1, list2);
        }
}

Test(daut, a_or_b)
{
    /*
     *      /--- 1->
     *     /  a
     *  ->0
     *     \  b
     *      \--- 2->
     */
    Automaton *expected = Automaton(3);
    State *q0 = State(0);
    State *q1 = State(1);
    State *q2 = State(1);
    automaton_add_state(expected, q0, 1);
    automaton_add_state(expected, q1, 0);
    automaton_add_state(expected, q2, 0);
    automaton_add_transition(expected, q0, q1, 'a', 0);
    automaton_add_transition(expected, q0, q2, 'b', 0);

    Automaton *a_or_b = automaton_from_daut(TEST_PATH "automaton/a+b.daut");
    ASSERT_AUTOMATON_EQ(a_or_b, expected);

    automaton_free(expected);
    automaton_free(a_or_b);
}

Test(daut, abba)
{
    /*
     *       a    b    b    a
     *  -> 0 -> 1 -> 2 -> 3 -> 4 ->
     */
    Automaton *expected = Automaton(6);
    State *q0 = State(0);
    State *q1 = State(0);
    State *q2 = State(0);
    State *q3 = State(0);
    State *q4 = State(0);
    State *q5 = State(1);
    automaton_add_state(expected, q0, 1);
    automaton_add_state(expected, q1, 1);
    automaton_add_state(expected, q3, 0);
    automaton_add_state(expected, q4, 0);
    automaton_add_state(expected, q2, 0);
    automaton_add_state(expected, q5, 0);
    automaton_add_transition(expected, q0, q1, 0, 1);
    automaton_add_transition(expected, q1, q2, 'a', 0);
    automaton_add_transition(expected, q2, q3, 'b', 0);
    automaton_add_transition(expected, q3, q4, 'b', 0);
    automaton_add_transition(expected, q4, q5, 'a', 0);

    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut");
    ASSERT_AUTOMATON_EQ(abba, expected);

    automaton_free(expected);
    automaton_free(abba);
}
