#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/automaton.h"

#define STRINGIZE_(x) #x
#define TEST_PATH STRINGIZE_(test/daut_files/)
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

    for (size_t i = 0; i < a1->size; i++)
    {
        LinkedList *adj1 = *(LinkedList **) array_get(a1->adj_lists, i);
        LinkedList *adj2 = *(LinkedList **) array_get(a2->adj_lists, i);
        while (adj1->next != NULL && adj2->next != NULL)
        {
            Transition *tr1 = adj1->next->data;
            Transition *tr2 = adj2->next->data;

            cr_assert_eq(tr1->target->id, tr2->target->id,
                         "First target: %zu, second target: %zu (line %zu)",
                         tr1->target->id, tr2->target->id, line);
            cr_assert_eq(tr1->value, tr2->value);
            cr_assert_eq(tr1->is_epsilon, tr2->is_epsilon);

            adj1 = adj1->next;
            adj2 = adj2->next;
        }
        cr_assert(adj1->next == NULL && adj2->next == NULL,
                  "The two automata don't have the same transitions"
                  " (line %zu)", line);
    }
}

//Test(daut, a_or_b)
//{
//    /*
//     *      /--- 1->
//     *     /  a
//     *  ->0
//     *     \  b
//     *      \--- 2->
//     */
//    Automaton *expected = automaton_create();
//    State *q0 = State(0);
//    State *q1 = State(1);
//    State *q2 = State(1);
//    automaton_add_state(expected, q0, 1);
//    automaton_add_state(expected, q1, 0);
//    automaton_add_state(expected, q2, 0);
//    automaton_add_transition(expected, q0, q1, 'a', 0);
//    automaton_add_transition(expected, q0, q2, 'b', 0);
//
//    Automaton *a_or_b = automaton_from_daut(TEST_PATH "a+b.daut");
//    ASSERT_AUTOMATON_EQ(a_or_b, expected);
//
//    automaton_free(expected);
//    automaton_free(a_or_b);
//}

Test(daut, abba)
{
    /*
     *       a    b    b    a
     *  -> 0 -> 1 -> 2 -> 3 -> 4 ->
     */
    Automaton *expected = automaton_create();
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

    Automaton *abba = automaton_from_daut(TEST_PATH "abba.daut");
    ASSERT_AUTOMATON_EQ(abba, expected);

    automaton_free(expected);
    automaton_free(abba);
}
