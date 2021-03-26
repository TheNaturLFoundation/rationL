#include <criterion/criterion.h>
#include <stdio.h>
#include "automaton/automaton.h"


Test(automaton, add_order_update)
{
    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(automaton->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state)
{
    Automaton * automaton = Automaton();
    State * to_add = State(0)
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(to_add->id, 0);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_adj_lists_check1)
{
    /*
    Checks if the array is expanded correcty
    */

    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    cr_assert_eq(automaton->adj_lists->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_adj_lists_check2)
{
    /*
    Checks if the empty adj_list is correctly initialized
    */

    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    LinkedList ** adj = array_get(automaton->adj_lists,0);
    cr_assert_eq((*adj)->next, NULL, "got %lu, expected 0\n", (*adj)->size);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_states)
{
    /*
    Checks if the state is correctly appended to the array of
    states
    */

    Automaton * automaton = Automaton();
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

    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 1);
    State ** elt = array_get(automaton->starting_states, 0); 
    cr_assert_eq(*elt, to_add, "got %p expected %p\n", elt, to_add);
    automaton_free(automaton);
}

Test(automaton, add_multpile)
{
    size_t entries = 0;
    Automaton * automaton = Automaton();
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
    cr_assert_eq(automaton->adj_lists->size, 100);
    cr_assert_eq(automaton->states->size, 100);
    cr_assert_eq(automaton->starting_states->size, entries);
    automaton_free(automaton);
}

Test(automaton, add_transition_no_epsilion)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    LinkedList ** adj1 = array_get(automaton->adj_lists, 0);

    LinkedList * elt = list_get(*adj1, 0);
    Transition * tr = elt->data;
    
    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", elt->next);
    cr_assert_eq(tr->target, s2, "got %p expected %p\n", tr->target, s2);
    cr_assert_eq(tr->value, 'A', "got '%c' expected 'A'\n", tr->value);
    cr_assert_eq(tr->is_epsilon, 0, "got %i expected 0", tr->is_epsilon);

    automaton_free(automaton);
}

Test(automaton, add_transition_no_epsilion2)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s2, s1, 'A', 0);

    LinkedList ** adj2 = array_get(automaton->adj_lists, 1);

    LinkedList * elt = list_get(*adj2, 0);
    Transition * tr = elt->data;
    
    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", elt->next);
    cr_assert_eq(tr->target, s1, "got %p expected %p\n", tr->target, s1);
    cr_assert_eq(tr->value, 'A', "got '%c' expected 'A'\n", tr->value);
    cr_assert_eq(tr->is_epsilon, 0, "got %i expected 0", tr->is_epsilon);

    automaton_free(automaton);
}

Test(automaton, add_transition_epsilon)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    LinkedList ** adj1 = array_get(automaton->adj_lists, 0);

    LinkedList * elt = list_get(*adj1, 0);
    Transition * tr = elt->data;
    
    cr_assert_eq(elt->next, NULL, "got %p expected NULL\n", elt->next);
    cr_assert_eq(tr->target, s2, "got %p expected %p\n", tr->target, s2);
    cr_assert_eq(tr->is_epsilon, 1, "got %i expected 0", tr->is_epsilon);

    automaton_free(automaton);
}

Test(automaton, add_multpile_tr)
{
    Automaton * automaton = Automaton();
    State * s;
    State * s4vj;
    State ** state_collector;
    LinkedList ** list_collector;
    LinkedList * list;
    Transition * tr;
    for(size_t i = 0; i < 500; i++)
    {
        s = State(0);
        automaton_add_state(automaton, s, 0);
    }
    state_collector = array_get(automaton->states, 42);
    s = *state_collector;
    for(size_t i = 0; i < 500; i++)
    {
        state_collector = array_get(automaton->states, i);
        s4vj = * state_collector;
        automaton_add_transition(automaton, s, s4vj, 'A', 0);
    }
    list_collector = array_get(automaton->adj_lists, 42);
    list = *list_collector;
    for(size_t i = 0; i < 500; i++)
    {
        state_collector = array_get(automaton->states, i);
        s = *state_collector;
        tr = list->next->data;
        list = list->next;
        cr_assert_eq(tr->target, s, "got %p, expected %p at %lu\n", tr->target, s, i);
    }
    size_t count = 0;
    list = *list_collector;
    while(list->next != NULL)
    {
        count++;
        list = list->next;
    }
    cr_assert_eq(count, 500, "got %lu instead of 500\n", count);

    for(size_t i = 0; i < 500; i ++)
    {
        if(i != 42)
        {
            list_collector = array_get(automaton->adj_lists, i);
            list = *list_collector;
            cr_assert_eq(list->next, NULL);
        }
    }

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition1)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, 'A', 0);
    int ret = automaton_remove_transition(automaton, s1, s2, 'A', 0);
    LinkedList ** list = array_get(automaton->adj_lists, 0);
    cr_assert_eq((*list)->next, NULL);
    list = array_get(automaton->adj_lists, 1);
    cr_assert_eq(ret, 0);
    cr_assert_eq((*list)->next, NULL);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transion2)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret =  automaton_remove_transition(automaton, s1, s2, 'A', 0);

    LinkedList ** list = array_get(automaton->adj_lists, 0);
    cr_assert_eq(ret, 0);
    cr_assert_eq((*list)->next->next, NULL);
    
    Transition * tr = (*list)->next->data;
    cr_assert_eq(tr->value, ' ');
    cr_assert_eq(tr->target, s2);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition3)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 0);

    int ret =  automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList ** list_ptr = array_get(automaton->adj_lists, 0);
    Transition * tr = (*list_ptr)->next->data;

    cr_assert_eq(ret, 0);
    cr_assert_eq((*list_ptr)->next->next, NULL);
    cr_assert_eq(tr->value, 'A');
    cr_assert_eq(tr->is_epsilon, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition4)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(1);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 0);
    automaton_add_transition(automaton, s1, s2, ' ', 1);
    automaton_add_transition(automaton, s1, s2, 'A', 1);

    int ret =  automaton_remove_transition(automaton, s1, s2, 's', 1);

    LinkedList ** list = array_get(automaton->adj_lists, 0);
    Transition * tr = (*list)->next->data;

    cr_assert_eq(ret, 0);
    cr_assert_eq((*list)->next->next, NULL);
    cr_assert_eq(tr->value, 'A');
    cr_assert_eq(tr->is_epsilon, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_transition_fail1)
{
    Automaton * automaton = Automaton();
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
    Automaton * automaton = Automaton();
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
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_remove_state(automaton, s1);

    cr_assert_eq(automaton->size, 1, "got %lu expected 1\n", automaton->size);
    cr_assert_eq(automaton->adj_lists->size, 1);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 1);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_entry)
{
    Automaton * automaton = Automaton();
    State * s1 = State(0);
    State * s2 = State(0);
    automaton_add_state(automaton, s1, 0);
    automaton_add_state(automaton, s2, 1);
    automaton_remove_state(automaton, s2);

    cr_assert_eq(automaton->size, 1);
    cr_assert_eq(automaton->adj_lists->size, 1);
    cr_assert_eq(automaton->states->size, 1);
    cr_assert_eq(automaton->starting_states->size, 0);

    automaton_free(automaton);
}

Test(automaton, automaton_remove_state_upgrade)
{
    Automaton * automaton = Automaton();
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

Test(automaton, automaton_remove_state_transitions)
{
    Automaton * automaton = Automaton();
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

    LinkedList * list = *(LinkedList **)array_get(automaton->adj_lists, 0);
    cr_assert_eq(list->next, NULL);
    list = *(LinkedList **)array_get(automaton->adj_lists, s3->id);
    Transition * tr = list->next->data;
    cr_assert_eq(tr->target, s4vj);

    automaton_free(automaton);
}
