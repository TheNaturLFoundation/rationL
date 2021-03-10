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

int automaton_remove_transition(Automaton * automaton, State * src, 
    State * dst, Letter value, int epsilon)
{
    LinkedList * list = *(LinkedList **)array_get(automaton->adj_lists, src->id);
    LinkedList * cpy = list;
    Transition * tr;
    size_t i = 0;
    while(list->next != NULL)
    {
        tr = list->next->data;
        if(tr->target == dst && ((epsilon == 1 && tr->is_epsilon == 1) || tr->value == value))
        {
            list_free(list_pop_at(cpy, i));
            return 0;
        }
        i++;
        list = list->next;
    }
    return 1;
}

void automaton_remove_state(Automaton * automaton, State * state)
{
    Transition * tr;
    size_t antoine;
    LinkedList * cpy;
    State * another_state;
    //Remove from adj_lists
    list_free(*(LinkedList **)array_get(automaton->adj_lists, state->id));
    array_remove(automaton->adj_lists, state->id);
    arr_foreach(LinkedList *, list, automaton->adj_lists)
    {
        antoine = 0;
        cpy = list;
        while(list->next != NULL)
        {
            tr = list->next->data;
            list = list->next;
            if(tr->target == state)
            {
                list_free(list_pop_at(cpy, antoine));
            }
            else
            {
                antoine++;
            }
        }
        
    }
    //Remove from starting_states
    antoine = 0;
    arr_foreach(State *, s, automaton->starting_states)
    {
        if(s == state)
        {
            array_remove(automaton->starting_states, antoine);
            break;
        }
        antoine++;
    }
    //Remove from states array
    for(size_t k = state->id + 1; k < automaton->states->size; k++)
    {
        another_state = *(State **)array_get(automaton->states, k);
        another_state->id -= 1;
    }
    array_remove(automaton->states, state->id);
    automaton->size -= 1;
    free(state);
}
