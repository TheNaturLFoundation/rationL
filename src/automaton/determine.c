#include "determine.h"

#include <printf.h>

#include "datatypes/map.h"

static void free_powersets(Map *);

Automaton *determine(const Automaton *source)
{
    Automaton *automaton = Automaton(1, source->lookup_used);
    // Associate each powerset to a state id
    Map *powersets = Map(Set *, size_t, &hash_set, &compare_sets);
    // For each iterations, collect the set of ids associated to each letter
    Map *state_sets = Map(Letter, Set *, &hash_char, &compare_uchars);
    LinkedList *set_queue = LinkedList(Set *);

    // Initialize the starting state
    int terminal = 0;
    Set *start_set = Set(size_t, &hash_size_t, &compare_size_t);
    arr_foreach(State *, starting_state, source->starting_states)
    {
        if (starting_state->terminal)
            terminal = 1;
        set_add(start_set, &starting_state->id);
    }
    // Push it in the queue
    State *first_state = State(terminal);
    automaton_add_state(automaton, first_state, 1);
    map_set(powersets, &start_set, &first_state->id);
    list_push_back(set_queue, &start_set);

    // Add states until we can't
    size_t current_index = 0;
    while (!list_empty(set_queue))
    {
        Set *current_set = *(Set **)list_pop_front_value(set_queue);
        size_t current_id = *(size_t *)map_get(powersets, &current_set);
        State *src_state = *(State **)array_get(automaton->states, current_id);

        map_clear(state_sets);
        // Initialize the `state_sets` dict
        map_foreach_key(size_t, state_id, current_set, {
            for (Letter c = 0; c < 255; c++)
            {
                LinkedList *list = get_matrix_elt(source, state_id, c, c == 0);
                if (list_empty(list))
                    continue;

                Set *set;
                void *ptr = map_get(state_sets, &c);
                if (ptr == NULL)
                {
                    set = Set(size_t, &hash_size_t, &compare_size_t);
                    map_set(state_sets, &c, &set);
                }
                else
                    set = *(Set **)ptr;

                list_foreach(State *, next, list)
                    set_add(set, &next->id);
            }
        });

        // For each set of states, if it does not exist,
        // create a corresponding state and map it.
        // Create a transition from the current state to the next one.
        map_foreach_key(Letter, value, state_sets, {
            Set *set = *(Set **)map_get(state_sets, &value);
            State *dst_state;
            void *ptr = map_get(powersets, &set);
            if (ptr == NULL)
            {
                // If at least one dst_state is terminal in the source,
                // the new dst_state is terminal.
                terminal = 0;
                map_foreach_key(size_t, id, set, {
                    State *state = *(State **)array_get(source->states, id);
                    if (state->terminal)
                    {
                        terminal = 1;
                        // map_foreach_key uses two loops, break isn't enough
                        goto out85;
                    }
                }) out85:;
                dst_state = State(terminal);
                automaton_add_state(automaton, dst_state, 0);
                map_set(powersets, &set, &dst_state->id);
                list_push_back(set_queue, &set);
            }
            else
            {
                size_t id = *(size_t *)ptr;
                dst_state = *(State **)array_get(automaton->states, id);
                map_free(set);
            }

            automaton_add_transition(automaton, src_state, dst_state, value, 0);
        });
    }

    // The contained sets are either freed or moveed to the powersets map
    // so there is no need to free them.
    map_free(state_sets);
    free_powersets(powersets);

    return automaton;
}

static void free_powersets(Map *powersets)
{
    map_foreach_key(Set *, set, powersets, map_free(set););
    map_free(powersets);
}
