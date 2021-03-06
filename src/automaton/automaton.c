#include "automaton.h"

Automaton * automaton_create()
{
    Automaton * autom = SAFEMALLOC(sizeof(Automaton));
    autom->size = 0;
    autom->adj_lists = Array(LinkedList);
    autom->starting_states = Array(State);
    return autom;
}