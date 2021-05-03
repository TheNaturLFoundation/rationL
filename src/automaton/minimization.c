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
        State *new_state = State(0);
        automaton_add_state(automaton, new_state, terminal);
    }

    arr_foreach(State *, start_state, source->starting_states)
    {
        State *new_state = *(State **)array_get(automaton->states, start_state->id);
        new_state->terminal = 1;
    }

    for(size_t j = 0; j < source->size; j++)
    {
        State *old_src = *(State **)(array_get(automaton->states, j));

        for (size_t letter = 0; letter < 256; letter++)
        {
            list_foreach(State *, old_dst, get_matrix_elt(source, j, letter, letter == 0))
            {
                State *new_dst = *(State **)array_get(automaton->states, old_dst->id);
                automaton_add_transition(automaton, new_dst, old_src, letter, letter == 0);
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
