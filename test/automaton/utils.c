#include "utils.h"
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

void assert_automaton_eq_(size_t line, Automaton *a1, Automaton *a2)
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

    cr_assert_eq(a1->lookup_used, a2->lookup_used,
                 "First table has %zu different states, "
                 "second has width %zu (line %zu)",
                 a1->lookup_used, a2->lookup_used, line);

    for (Letter c = 0; c < a1->lookup_used; c++)
        for (size_t src = 0; src < a1->size; src++)
        {
            int is_epsilon = src == 0;
            LinkedList *list1 = get_matrix_elt(a1, src, c, is_epsilon);
            LinkedList *list2 = get_matrix_elt(a2, src, c, is_epsilon);
            if (list1 == NULL || list2 == NULL)
                cr_assert_eq(list1, list2, "%p != %p", list1, list2);
            else
            {
                list1 = list1->next;
                list2 = list2->next;
            }
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
