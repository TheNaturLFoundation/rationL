#include <criterion/criterion.h>
#include <stdio.h>
#include "rationl/automaton.h"


Test(automaton, empty_init_dimens)
{
    Automaton *automaton = Automaton(1);
    cr_assert_eq(automaton->transition_table->width, 256);
    cr_assert_eq(automaton->transition_table->height, 1);
    cr_assert_eq(automaton->size, 0);
    automaton_free(automaton);
}

Test(automaton, empty_init_sentinels)
{
    Automaton *automaton = Automaton(2);
    for (size_t i = 0; i < automaton->transition_table->width; i++)
        cr_assert_eq(matrix_get(automaton->transition_table, i, 1)->next, NULL);
    automaton_free(automaton);
}

Test(automaton, add_order_update)
{
    Automaton * automaton = Automaton(1);
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(automaton->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state)
{
    Automaton * automaton = Automaton(1);
    State * to_add = State(0)
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(to_add->id, 0);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_empty_row)
{
    /*
    Checks if the row is still empty
    */

    Automaton * automaton = Automaton(1);
    State * to_add = State(0);
    for (size_t i = 0; i < automaton->transition_table->width; i++)
        cr_assert_eq(matrix_get(automaton->transition_table, i, 0)->next, NULL);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_states)
{
    /*
    Checks if the state is correctly appended to the array of
    states
    */

    Automaton * automaton = Automaton(1);
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    State ** s = array_get(automaton->states, 0);
    cr_assert_eq(*s, to_add);
    automaton_free(automaton);
}

Test(automaton, add_starting_state)
{
    /*
    Checks if the entry state is correctly added to the
    starting_states array of automaton
    */

    Automaton * automaton = Automaton(1);
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 1);
    State ** elt = array_get(automaton->starting_states, 0);
    cr_assert_eq(*elt, to_add, "got %p expected %p\n", (void*) elt, (void*) to_add);
    automaton_free(automaton);
}

Test(automaton, add_multpile)
{
    size_t entries = 0;
    Automaton * automaton = Automaton(100);
    State * s;
    for(size_t i = 0; i < 100; i++)
    {
        s = State(0);
        if(i % 10 == 0)
        {
            automaton_add_state(automaton, s, 1);
            entries++;
        }
        else
        {
            automaton_add_state(automaton, s, 0);
        }
    }
    cr_assert_eq(automaton->size, 100);
    cr_assert_eq(automaton->states->size, 100);
    cr_assert_eq(automaton->starting_states->size, entries);
    automaton_free(automaton);
}

Test(automaton, add_transition_no_epsilion)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    LinkedList *list = matrix_get(automaton->transition_table, 'A', s1->id);
    LinkedList *elt = list->next;
    State *target = *(State **)elt->data;

    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", (void*) elt->next);
    cr_assert_eq(target->id, s2->id, "got %zu expected %zu\n", target->id, s2->id);

    automaton_free(automaton);
}

Test(automaton, add_transition_no_epsilion2)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'A', 0);

    LinkedList *list = matrix_get(automaton->transition_table, 'A', s2->id);
    LinkedList *elt = list->next;
    State *target = *(State **)elt->data;

    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", (void*) elt->next);
    cr_assert_eq(target->id, s1->id, "got %zu expected %zu\n", target->id, s1->id);

    automaton_free(automaton);
}

Test(automaton, add_transition_epsilon)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    // Epsilon transition
    LinkedList *list = matrix_get(automaton->transition_table, 0, s1->id);
    LinkedList *elt = list->next;
    State *target = *(State **)elt->data;

    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", (void*) elt->next);
    cr_assert_eq(target->id, s2->id, "got %zu expected %zu\n", target->id, s2->id);

    // A transition (none expected)
    list = matrix_get(automaton->transition_table, 'A', s1->id);
    cr_assert_eq(list->next, NULL, "got %p, expected NULL", (void *)list->next);

    automaton_free(automaton);
}

Test(automaton, add_multiple_tr)
{
    Automaton * automaton = Automaton(500);
    State * s;
    State * s4vj;
    State ** state_collector;

    for(size_t i = 0; i < 500; i++)
    {
        s = State(0);
        automaton_add_state(automaton, s, 0);
    }
    state_collector = array_get(automaton->states, 42);
    s = *state_collector;
    cr_assert_eq(s->id, 42, "got %zu, expected 42", s->id);
    for(size_t i = 0; i < 500; i++)
    {
        state_collector = array_get(automaton->states, i);
        s4vj = * state_collector;
        automaton_add_transition(automaton, s, s4vj, 'A', 0);
    }

    LinkedList *list = matrix_get(automaton->transition_table, 'A', s->id);
    size_t list_size = 0;
    list_foreach(State *, _s, list)
        list_size++;
    cr_assert_eq(list_size, 500, "got %zu, expected 500", list_size);

    for(size_t i = 0; i < 500; i == s->id - 1 ? i += 2 : i++)
    {
        list = matrix_get(automaton->transition_table, 'A', i);
        cr_assert(list_empty(list), "list isn't empty!");
    }

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition1)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);
    LinkedList *list = matrix_get(automaton->transition_table, 'A', s1->id);

    cr_assert_eq(ret, 0);
    cr_assert_eq(list->next, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transion2)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

    LinkedList *list = matrix_get(automaton->transition_table, 0, s1->id);
    State *target = *(State **)list->next->data;

    cr_assert_eq(ret, 0);
    cr_assert_eq(list->next->next, NULL);
    cr_assert_eq(target->id, s2->id);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition3)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList *list = matrix_get(automaton->transition_table, 'A', s1->id);
    State *target = *(State **)list->next->data;

    cr_assert_eq(ret, 0);
    cr_assert_eq(list->next->next, NULL);
    cr_assert_eq(target->id, s2->id);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition4)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    int ret = automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList *list = matrix_get(automaton->transition_table, 0, s1->id);
    State *target = *(State **)list->next->data;

    cr_assert_eq(ret, 0);
    cr_assert_eq(list->next->next, NULL);
    cr_assert_eq(target->id, s2->id);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail1)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 1);

    cr_assert_eq(ret, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail2)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);

    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);

    cr_assert_eq(ret, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_no_entry)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_remove_state(automaton, s1);

    cr_assert_eq(automaton->size, 1, "got %lu, expected 1\n", automaton->size);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_entry)
{
    Automaton * automaton = Automaton(2);
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_remove_state(automaton, s2);

    cr_assert_eq(automaton->size, 1);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_upgrade)
{
    Automaton * automaton = Automaton(4);
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(1);
    State * s4vj = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4vj, 1);
    automaton_add_transition(automaton, s2, s1, 'C', 0);

    automaton_remove_state(automaton, s1);

    cr_assert_eq(s2->id, 0);
    cr_assert_eq(s3->id, 1);
    cr_assert_eq(s4vj->id, 2);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_transitions, .disabled = 1)
{
    Automaton * automaton = Automaton(4);
    State * s1 = State(0);
    State * s2 = State(0);
    State * s3 = State(1);
    State * s4vj = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_add_state(automaton, s3, 0);
    automaton_add_state(automaton, s4vj, 1);
    automaton_add_transition(automaton, s2, s1, 'C', 0);
    automaton_add_transition(automaton, s3, s4vj, 'D', 0);
    automaton_add_transition(automaton, s3, s1, ' ', 1);
    automaton_add_transition(automaton, s3, s2, ' ', 1);
    automaton_remove_state(automaton, s1);

    for (size_t c = 0; c < automaton->transition_table->width; c++)
    {
        LinkedList *list = matrix_get(automaton->transition_table, c, 0);
        cr_assert_eq(list->next, NULL);
    }

    LinkedList *list = matrix_get(automaton->transition_table, 'D', s3->id);
    State *target = *(State **)list->next->data;
    cr_assert_eq(target->id, s4vj->id);

    automaton_free(automaton);
}
