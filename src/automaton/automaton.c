#include "automaton.h"

Automaton * automaton_create()
{
    Automaton * autom = SAFEMALLOC(sizeof(Automaton));
    autom->size = 0;
    autom->adj_lists = Array(LinkedList *);
    autom->starting_states = Array(State *);
    autom->states = Array(State *);
    autom->is_determined = 0;
    return autom;
}

void automaton_free(Automaton * automaton)
{
    arr_foreach(State *, s, automaton->states)
    {
        //Frees the states.
        free(s);
    }
    arr_foreach(LinkedList *, list, automaton->adj_lists)
    {
        //Frees the transition list
        list_free(list);
    }
    array_free(automaton->states);
    array_free(automaton->adj_lists);
    array_free(automaton->starting_states);
    free(automaton);
}

State * state_create(int is_terminal)
{
    State * s = SAFEMALLOC(sizeof(State));
    s->terminal = is_terminal;
    return s;
}

void automaton_add_state(Automaton * automaton, State * state, int is_entry)
{
    LinkedList * new_state_list = LinkedList(Transition);
    array_append(automaton->adj_lists, &new_state_list);
    array_append(automaton->states, &state);
    state->id = automaton->size;
    automaton->size++;
    if(is_entry == 1)
    {
        array_append(automaton->starting_states, &state);
    }
}

void automaton_add_transition(Automaton * automaton, State * src, 
    State *dst, Letter value, int epsilon)
{
    LinkedList ** adj_list = array_get(automaton->adj_lists, src->id);
    Transition tr;
    tr.target = dst;
    tr.value = value;
    tr.is_epsilon = epsilon;
    if(list_push_back(*adj_list, &tr) == 0)
    {
        err(1, "Unable to append to the list at address %p", adj_list);
    }
}