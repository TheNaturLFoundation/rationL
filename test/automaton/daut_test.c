#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "utils.h"

Test(daut, a_or_b)
{
    /*
     *      /--- 1->
     *     /  a
     *  ->0
     *     \  b
     *      \--- 2->
     */
    Automaton *expected = Automaton(3, 2);
    State *q0 = State(0);
    State *q1 = State(1);
    State *q2 = State(1);
    automaton_add_state(expected, q0, 1);
    automaton_add_state(expected, q1, 0);
    automaton_add_state(expected, q2, 0);
    automaton_add_transition(expected, q0, q1, 'a', 0);
    automaton_add_transition(expected, q0, q2, 'b', 0);

    Automaton *a_or_b = automaton_from_daut(TEST_PATH "automaton/a+b.daut", 3);
    ASSERT_AUTOMATON_EQ(a_or_b, expected);

    automaton_free(expected);
    automaton_free(a_or_b);
}


Test(daut, abba)
{
    /*
     *    ε    a    b    b    a
     *  0 -> 1 -> 2 -> 3 -> 4 -> 5 ->
     *       ^
     */
    Automaton *expected = Automaton(6, 3);
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

    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut", 6);
    ASSERT_AUTOMATON_EQ(abba, expected);

    automaton_free(expected);
    automaton_free(abba);
}

static void assert_set_eq(Set *s1, Set *s2)
{
    if (s1 == NULL || s2 == NULL)
        cr_assert_eq(s1, s2, "s1 == %p, s2 == %p", s1, s2);
    cr_assert_eq(s1->buckets->size, s2->buckets->size);

    for (size_t i = 0; i < s1->buckets->size; i++)
    {
        LinkedList *bucket1 = *(LinkedList **)array_get(s1->buckets, i);
        LinkedList *bucket2 = *(LinkedList **)array_get(s2->buckets, i);
        while (bucket1->next != NULL && bucket2->next != NULL)
        {
            MapNode *node1 = *(MapNode **)bucket1->next->data;
            MapNode *node2 = *(MapNode **)bucket2->next->data;

            size_t *x1 = node1->key;
            size_t *x2 = node2->key;
            cr_assert_eq(*x1, *x2);

            bucket1 = bucket1->next;
            bucket2 = bucket2->next;
        }
    }
}

Test(daut, special_transitions)
{
    Automaton *expected = Automaton(4, 3);
    State *q0 = State(0);
    State *q1 = State(0);
    State *q2 = State(0);
    State *q3 = State(1);
    automaton_add_state(expected, q0, 1);
    automaton_add_state(expected, q1, 0);
    automaton_add_state(expected, q2, 0);
    automaton_add_state(expected, q3, 0);
    automaton_add_transition(expected, q0, q1, 'a', 0);
    automaton_add_transition(expected, q1, q2, 'b', 0);
    automaton_add_transition(expected, q2, q3, 'c', 0);
    automaton_mark_entering(expected, NULL, q0, 0, 0, 0);
    automaton_mark_entering(expected, q0, q1, 'a', 0, 1);
    automaton_mark_entering(expected, q1, q2, 'b', 0, 2);
    automaton_mark_leaving(expected, q3, NULL, 0, 0, 0);
    automaton_mark_leaving(expected, q3, NULL, 0, 0, 1);
    automaton_mark_leaving(expected, q3, NULL, 0, 0, 2);

    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/(a(b(c))).daut", 4);
    State *r0 = *(State **)array_get(aut->states, 0);
    State *r1 = *(State **)array_get(aut->states, 1);
    State *r2 = *(State **)array_get(aut->states, 2);
    State *r3 = *(State **)array_get(aut->states, 3);

    assert_set_eq(get_entering_groups(expected, NULL, q0, 0, 0),
                      get_entering_groups(aut, NULL, r0, 0, 0));
    assert_set_eq(get_entering_groups(expected, q0, q1, 'a', 0),
                  get_entering_groups(aut, r0, r1, 'a', 0));
    assert_set_eq(get_entering_groups(expected, q1, q2, 'b', 0),
                  get_entering_groups(aut, r1, r2, 'b', 0));
    assert_set_eq(get_leaving_group(expected, q3, NULL, 0, 0),
                  get_leaving_group(aut, q3, NULL, 0, 0));

    automaton_free(aut);
}
