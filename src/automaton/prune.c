#include "automaton/prune.h"
#include "utils/memory_utils.h"

size_t _automaton_prune(Automaton * automaton, State * s, size_t * pref, int * escape, State ** higher, size_t * cpt)
{
    LinkedList * list;
    State * adj;
    (*cpt)++;
    escape[s->id] = s->terminal;
    higher[s->id] = s;
    pref[s->id] = *cpt;
    size_t hpref = *cpt;
    size_t adj_hpref;

    for(size_t c = 0; c < automaton->transition_table->width; c++)
    {
        list = matrix_get(automaton->transition_table, c, s->id);
        if(list != NULL)
        {
            while(list->next != NULL)
            {
                adj = *(State **)list->next->data;
                if(higher[adj->id] == NULL)
                {
                    adj_hpref = _automaton_prune(automaton, adj, pref, escape, higher, cpt);
                    if(adj_hpref < hpref)
                    {
                        hpref = adj_hpref;
                        higher[s->id] = higher[adj->id];
                    }
                }
                else if(pref[s->id] > pref[adj->id])
                {
                    higher[s->id] = higher[adj->id];
                    hpref = pref[adj->id];
                }
                escape[s->id] = escape[s->id] | escape[adj->id];
                list = list->next;
            }
        }
    }
    return hpref;
}

void _rec_update(State ** higher, int * escape, State * x)
{
    if(higher[x->id] != NULL && higher[x->id] != x)
    {
        _rec_update(higher, escape, higher[x->id]);
        escape[x->id] |= escape[higher[x->id]->id];
        higher[x->id] = x;   
    }
}
 
void automaton_prune(Automaton * automaton)
{
    size_t * pref = SAFECALLOC(automaton->size, sizeof(size_t));
    int * escape = SAFECALLOC(automaton->size, sizeof(int));
    State ** higher = SAFECALLOC(automaton->size, sizeof(State *));
    Array * to_delete = Array(State *);
    size_t cpt = 0;

    arr_foreach(State *, entry, automaton->starting_states)
    {
        if(higher[entry->id] == NULL)
        {
            _automaton_prune(automaton, entry, pref, escape, higher, &cpt);
        }
    }
    
    arr_foreach(State *, state, automaton->states)
    {
        _rec_update(higher, escape, state);
    }

    arr_foreach(State *, s, automaton->states)
    {
        if((higher[s->id] == NULL) || (escape[s->id] == 0))
        {
            array_append(to_delete, &s);
        }
    }

    arr_foreach(State *, s_r, to_delete)
    {
        automaton_remove_state(automaton, s_r);
    }

    free(higher);
    free(pref);
    free(escape);
    array_free(to_delete);
}

