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
