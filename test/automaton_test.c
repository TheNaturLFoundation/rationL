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
    cr_assert_eq(automaton->starting_states->size, entries);
    automaton_free(automaton);
}

/*
Test(automaton, remove_state_order_update)
{
    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    automaton_remove_state(automaton, to_add);
    cr_assert_eq(automaton->size, 0);
    automaton_free(automaton);
}
*/
/*
Test(automaton, remove_state_normal)
{
    
    Verifies if after removing from the automaton is empty
    again
    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 0);
    automaton_remove_state(automaton, to_add);
    size_t n = automaton->adj_lists->size;
    cr_assert_eq(n, 0);
    automaton_free(automaton);
}
*/
/*
Test(automaton, remove_state_entry)
{
    /*
    Verifies if after removing from the automaton the
    starting_states array is updated
    Automaton * automaton = Automaton();
    State * to_add = State(0);
    automaton_add_state(automaton, to_add, 1);
    automaton_remove_state(automaton, to_add);
    size_t n = automaton->starting_states->size;
    cr_assert_eq(n, 0);
    automaton_free(automaton);
}
    */