#include <criterion/criterion.h>
#include <stdio.h>
#include "automaton/automaton.c"

Test(automaton, add_order_update)
{
    Automaton * automaton = Automaton();
    State to_add;
    automaton_add_state(automaton, &to_add, 0);
    cr_assert_eq(automaton->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state)
{
    Automaton * automaton = Automaton();
    State to_add;
    to_add.terminal = 0;
    automaton_add_state(automaton, &to_add, 0);
    cr_assert_eq(to_add.id, 0);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_adj_lists_check1)
{
    /*
    Checks if the array is expanded correcty
    */
    Automaton * automaton = Automaton();
    State to_add;
    to_add.terminal = 0;
    automaton_add_state(automaton, &to_add, 0);
    cr_assert_eq(automaton->adj_lists->size, 1);
    automaton_free(automaton);
}

Test(automaton, add_classic_state_adj_lists_check2)
{
    /*
    Checks if the empty adj_list is correctly initialized
    */
    Automaton * automaton = Automaton();
    State to_add;
    to_add.terminal = 0;
    automaton_add_state(automaton, &to_add, 0);
    LinkedList adj = array_get(automaton->adj_lists,0);
    cr_assert_eq(adj->size, 0);
    automaton_free(automaton);
}

Test(automaton, add_starting_state)
{
    /*
    Checks if the entry state is correctly added to the 
    starting_states array of automaton
    */
    Automaton * automaton = Automaton();
    State to_add;
    to_add.terminal = 0;
    automaton_add_state(automaton, &to_add, 1);
    State * elt = array_get(automaton->starting_states, 0); 
    cr_assert_eq(elt, &to_add);
    automaton_free(automaton);
}

Test(automaton, remove_state_order_update)
{
    Automaton * automaton = Automaton();
    State to_add;
    automaton_add_state(automaton, &to_add, 0);
    automaton_remove_state(automaton, &to_add);
    cr_assert_eq(automaton->size, 0);
    automaton_free(automaton);
}

Test(automaton, remove_state_normal)
{
    /*
    Verifies if after removing from the automaton is empty
    again
    */
    Automaton * automaton = Automaton();
    State to_add;
    automaton_add_state(automaton, &to_add, 0);
    automaton_remove_state(automaton, &to_add);
    size_t n = automaton->adj_lists->size;
    cr_assert_eq(n, 0);
    automaton_free(automaton);
}

Test(automaton, remove_state_entry)
{
    /*
    Verifies if after removing from the automaton the
    starting_states array is updated
    */
    Automaton * automaton = Automaton();
    State to_add;
    automaton_add_state(automaton, &to_add, 1);
    automaton_remove_state(automaton, &to_add);
    size_t n = automaton->starting_states->size;
    cr_assert_eq(n, 0);
    automaton_free(automaton);
}