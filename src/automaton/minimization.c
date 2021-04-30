#include "minimization.h"
#include "datatypes/array.h"
#include "automaton.h"
#include "determine.h"
#include "delete_eps.h"
#include "prune.h"

Automaton *transpose(Automaton *source)
{
    Automaton *automaton = automaton_create(source->size, 256);
    automaton->capacity = source->capacity;
    automaton->lookup_used = source->lookup_used;

    arr_foreach(State *, state, source->states)
    {
        int terminal = state->terminal;
        state->terminal = 0;
        automaton_add_state(automaton, state, terminal);
    }

    arr_foreach(State *, start_state, source->starting_states)
    {
        start_state->terminal = 1;
    }

    for(size_t j = 0; j < source->size; j++)
    {
        State *old_src = *(State **)(array_get(automaton->states, j));

        for (size_t letter = 0; letter < 256; letter++)
        {
            list_foreach(State *, old_dst, get_matrix_elt(source, j, letter, letter == 0))
            {
                automaton_add_transition(automaton, old_dst, old_src, letter, letter == 0);
            }
        }
    }

    return automaton;
}

Automaton *minimize(Automaton *source)
{
    source = transpose(source);
    source = determine(source);
    source = transpose(source);
    source = determine(source);

    return source;
}
