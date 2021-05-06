#include "minimization.h"
#include "datatypes/array.h"
#include "automaton.h"
#include "determine.h"
#include "delete_eps.h"
#include "prune.h"

typedef struct Transition
{
    size_t old_src;
    size_t old_dst;
    Letter letter;
}Transition;

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

#include <stdio.h>

void tr(Automaton *source)
{
    Array *new_transitions = Array(Transition);
    Array *new_starting_states = Array(size_t);

    arr_foreach(State *, state_src, source->states)
    {
        // Add terminal states to the new_stating_states
        if (state_src->terminal)
        {
            state_src->terminal = 0;
            array_append(new_starting_states, &state_src->id);
        }

        // Update the new_transitions list
        for (int i = 0; i < 256; i++)
        {
            if (source->lookup_table[i] != -1)
            {
                LinkedList *transitions =
                    matrix_get(source->transition_table,
                               source->lookup_table[i], state_src->id);
                if (transitions == NULL)
                    continue;

                list_foreach(State *, state_dst, transitions)
                {
                    //Add the reversed transition to transitions
                    Transition transition;
                    transition.old_src = state_src->id;
                    transition.old_dst = state_dst->id;
                    transition.letter = i;
                    array_append(new_transitions, &transition);

                    // Remove the old transition
                    //automaton_remove_transition(source, state_src, state_dst, i, i==0);
                }

                //list_free(transitions);
            }
        }
    }

    // Switch from starting state to terminal
    arr_foreach(State *, st, source->starting_states)
        st->terminal = 1;

    // Remove previous starting states
    array_clear(source->starting_states);

    // Add new starting states
    arr_foreach(size_t, position, new_starting_states)
    {
        array_append(source->starting_states, (State **)array_get(source->states, position));
    }

    arr_foreach(Transition, tr, new_transitions)
    {
        automaton_remove_transition(source,
                                    *(State **)array_get(source->states, tr.old_src),
                                    *(State **)array_get(source->states, tr.old_dst),
                                    tr.letter, tr.letter==0);
        automaton_add_transition(source,
                                 *(State **)array_get(source->states, tr.old_dst),
                                 *(State **)array_get(source->states, tr.old_src),
                                 tr.letter, tr.letter==0);
    }
}

Automaton *minimize(Automaton *source)
{
    tr(source);
    Automaton *atm1;
    atm1 = determine(source);
    //automaton_free(source);

    tr(atm1);
    Automaton *atm2;
    atm2 = determine(atm1);
    automaton_free(atm1);

    return atm2;
}
