#include "automaton/prune.h"
#include "utils/memory_utils.h"

void _automaton_prune(Automaton * automaton, State * s, int * access, int * escape)
{
    LinkedList * list;
    State * adj;

    access[s->id] = 1;
    escape[s->id] = s->terminal;

    for(size_t c = 0; c < automaton->transition_table->width; c++)
    {
        list = matrix_get(automaton->transition_table, c, s->id);
        if(list != NULL)
        {
            while(list->next != NULL)
            {
                adj = *(State **)list->next->data;
                if(access[adj->id] == 0)
                    _automaton_prune(automaton, adj, access, escape);
                escape[s->id] = escape[s->id] | escape[adj->id];
                list = list->next;
            }
        }
    }
}


void automaton_prune(Automaton * automaton)
{
    int * access = SAFECALLOC(automaton->size, sizeof(int));
    int * escape = SAFECALLOC(automaton->size, sizeof(int));
    Array * to_delete = Array(State *);

    arr_foreach(State *, entry, automaton->starting_states)
    {
        if(access[entry->id] == 0)
        {
            _automaton_prune(automaton, entry, access, escape);
        }
    }

    arr_foreach(State *, s, automaton->states)
    {
        if((access[s->id] == 0) || (escape[s->id] == 0))
        {
            array_append(to_delete, &s);
        }
    }

    arr_foreach(State *, s_r, to_delete)
    {
        automaton_remove_state(automaton, s_r);
    }

    free(access);
    free(escape);
    array_free(to_delete);
}
