#include "utils.h"

#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

static void assert_set_eq(size_t line, Set *s1, Set *s2);

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
        {
            if (s2->id == s1->id)
            {
                found = 1;
                break;
            }
        }
        cr_assert(found,
                  "First automaton has state %zu as a starting state,"
                  " second hasn't (line %zu)",
                  s1->id, line);

        s2 = *(State **)array_get(a2->starting_states, i);
        assert_set_eq(line, get_entering_groups(a1, NULL, s1, 0, 0),
                      get_entering_groups(a2, NULL, s2, 0, 0));
    }
    cr_assert_eq(a1->states->size, a2->states->size,
                 "First automaton has %zu states, second has %zu (line %zu)",
                 a1->states->size, a2->states->size, line);

    size_t terminal1 = 0;
    size_t terminal2 = 0;
    arr_foreach(State *, s1, a1->states) terminal1 += s1->terminal;
    arr_foreach(State *, s2, a2->states) terminal2 += s2->terminal;
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
        arr_foreach(State *, s2,
                    a2->states) if (s2->terminal && s2->id == s1->id)
        {
            found = 1;
            break;
        }
        cr_assert(found,
                  "First automaton has state %zu as a terminal state,"
                  " second hasn't (line %zu)",
                  s1->id, line);
    }

    cr_assert_eq(a1->size, a2->size,
                 "First automaton has size %zu, second has size %zu (line %zu)",
                 a1->size, a2->size, line);

    cr_assert_eq(a1->lookup_used, a2->lookup_used,
                 "First table has %zu different states, "
                 "second has width %zu (line %zu)",
                 a1->lookup_used, a2->lookup_used, line);

    for (size_t c = 0; c < NUMBER_OF_SYMB; c++)
        for (size_t src = 0; src < a1->size; src++)
        {
            int is_epsilon = c == EPSILON_INDEX;
            State *s1 = *(State **)array_get(a1->states, src);
            State *s2 = *(State **)array_get(a2->states, src);
            LinkedList *list1 = get_matrix_elt(a1, src, c, is_epsilon);
            LinkedList *list2 = get_matrix_elt(a2, src, c, is_epsilon);
            if (list1 == NULL || list2 == NULL)
                cr_assert_eq(list1, list2, "%p != %p (line: %zu)", list1, list2,
                             line);
            else
            {
                list1 = list1->next;
                list2 = list2->next;
            }
            while (list1 != NULL && list2 != NULL)
            {
                State *d1 = *(State **)list1->data;
                State *d2 = *(State **)list2->data;
                cr_assert_eq(d1->id, d2->id);
                cr_assert_eq(d1->terminal, d2->terminal);

                assert_set_eq(line, get_entering_groups(a1, s1, d1, c, is_epsilon),
                              get_entering_groups(a2, s2, d2, c, is_epsilon));
                assert_set_eq(line, get_leaving_group(a1, s1, d1, c, is_epsilon),
                              get_leaving_group(a2, s2, d2, c, is_epsilon));

                list1 = list1->next;
                list2 = list2->next;
            }
            cr_assert_eq(list1, list2);

            if (s1->terminal)
            {
                assert_set_eq(line, get_leaving_group(a1, s1, NULL, c, is_epsilon),
                              get_leaving_group(a2, s2, NULL, c, is_epsilon));
            }
        }
}

static void assert_set_eq(size_t line, Set *s1, Set *s2)
{
    if (s1 == NULL || s2 == NULL)
    {
        cr_assert_eq(s1, s2, "s1 == %p, s2 == %p (line %zu)", s1, s2, line);
        return;
    }
    if (s1->buckets == NULL || s2->buckets == NULL)
    {
        cr_assert_eq(s1->buckets, s2->buckets,
                     "s1->buckets == %p, s2->buckets == %p (line %zu)", s1, s2,
                     line);
        return;
    }
    cr_assert_eq(
        s1->buckets->size, s2->buckets->size,
        "first bucket has size %zu, second bucket has size %zu (line %zu)",
        s1->buckets->size, s2->buckets->size, line);

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
            cr_assert_eq(*x1, *x2, "%zu != %zu (line %zu)\n", *x1, *x2, line);

            bucket1 = bucket1->next;
            bucket2 = bucket2->next;
        }
        cr_assert_eq(bucket1->next, bucket2->next,
                     "First bucket is greater than the second (line %zu)\n",
                     line);
    }
}
