#include "determine.h"

#include <printf.h>

#include "datatypes/map.h"

static void free_powersets(Map *);

Automaton *determine(const Automaton *source)
{
    Automaton *automaton = Automaton(1, source->lookup_used);
    automaton->is_determined = 1;
    // Associate each powerset to a state id
    Map *powersets = Map(Set *, size_t, &hash_set, &compare_sets);
    // For each iterations, collect the set of ids associated to each letter
    Map *state_sets = Map(Letter, Set *, &hash_char, &compare_uchars);
    LinkedList *set_queue = LinkedList(Set *);

    // Initialize the starting state
    int terminal = 0;
    Set *start_set = Set(size_t, &hash_size_t, &compare_size_t);
    Set *entering_groups = Set(size_t, &hash_size_t, &compare_size_t);
    arr_foreach(State *, starting_state, source->starting_states)
    {
        if (starting_state->terminal)
            terminal = 1;
        set_add(start_set, &starting_state->id);

        Set *groups = get_entering_groups((Automaton *)source, NULL,
                                          starting_state, 0, 1);
        if (groups != NULL)
        {
            map_foreach_key(size_t, group, groups,
                            set_add(entering_groups, &group);)
        }
    }
    // Push it in the queue
    State *first_state = State(terminal);
    automaton_add_state(automaton, first_state, 1);
    {
        map_foreach_key(size_t, group, entering_groups, {
            automaton_mark_entering(automaton, NULL, first_state, 0, 0, group);
        });
    }
    map_free(entering_groups);
    map_set(powersets, &start_set, &first_state->id);
    list_push_back(set_queue, &start_set);

    // Add states until we can't
    while (!list_empty(set_queue))
    {
        Set *current_set = *(Set **)list_pop_front_value(set_queue);
        size_t current_id = *(size_t *)map_get(powersets, &current_set);
        State *src_state = *(State **)array_get(automaton->states, current_id);

        map_clear(state_sets);

        // Initialize the `state_sets` dict
        { map_foreach_key(
            size_t, state_id, current_set,
            {
                for (Letter c = 0; c < 255; c++)
                {
                    LinkedList *list =
                        get_matrix_elt(source, state_id, c, c == 0);
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

                    list_foreach(State *, next, list) set_add(set, &next->id);
                }
            }) }

        // For each set of states, if it does not exist,
        // create a corresponding state and map it.
        // Create a transition from the current state to the next one.
        map_foreach_key(Letter, value, state_sets, {
            Set *set = *(Set **)map_get(state_sets, &value);
            State *dst_state;
            void *ptr = map_get(powersets, &set);
            int free_set = 0;
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
                        // map_foreach_key uses two loops, break isn't
                        // enough
                        goto out101;
                    }
                }) out101:;
                dst_state = State(terminal);
                automaton_add_state(automaton, dst_state, 0);
                map_set(powersets, &set, &dst_state->id);
                list_push_back(set_queue, &set);
            }
            else
            {
                free_set = 1; // Don't free it here: we need it later
                size_t id = *(size_t *)ptr;
                dst_state = *(State **)array_get(automaton->states, id);
            }

            automaton_add_transition(automaton, src_state, dst_state, value, 0);

            map_foreach_key(size_t, src_id, current_set, {
                State *src = *(State **)array_get(source->states, src_id);
                map_foreach_key(size_t, dst_id, set, {
                    State *dst = *(State **)array_get(source->states, dst_id);

                    Set *groups = get_entering_groups((Automaton *)source, src,
                                                      dst, value, 0);
                    if (groups != NULL)
                    {
                        map_foreach_key(size_t, group, groups, {
                            automaton_mark_entering(automaton, src_state,
                                                    dst_state, value, 0, group);
                        })
                    }

                    groups = get_leaving_group((Automaton *)source, src, dst,
                                               value, 0);
                    if (groups != NULL)
                    {
                        map_foreach_key(size_t, group, groups, {
                            automaton_mark_leaving(automaton, src_state,
                                                   dst_state, value, 0, group);
                        })
                    }
                });
            });

            map_foreach_key(size_t, dst_id, set, {
                State *dst = *(State **)array_get(source->states, dst_id);
                if (dst->terminal)
                {
                    Set *groups =
                        get_leaving_group((Automaton *)source, dst, NULL, 0, 0);
                    if (groups != NULL)
                    {
                        map_foreach_key(size_t, group, groups, {
                            automaton_mark_leaving(automaton, dst_state, NULL,
                                                   value, 0, group);
                        })
                    }
                }
            });

            if (free_set)
                map_free(set);
        })
    }

    list_free(set_queue);

    // The contained sets are either freed or moved to the powersets map
    // so there is no need to free them.
    map_free(state_sets);
    free_powersets(powersets);
    return automaton;
}

Automaton *build_search_dfa(Automaton *source)
{
    typedef struct context
    {
        size_t offset;
        State *pref;
        State *curr;
    } context;

    Automaton *aut = automaton_copy(source);
    aut->is_determined = 1;

    Letter letter_mapping[aut->transition_table->width];
    {
        size_t index = 0;
        for (size_t i = 0; i < NUMBER_OF_SYMB; i++)
            if (aut->lookup_table[i] != -1)
                letter_mapping[index++] = i;
    }
    int visited[aut->size];
    for (size_t i = 0; i < aut->size; i++)
        visited[i] = 0;

    State *start = *(State **)array_get(aut->starting_states, 0);
    LinkedList *stack = LinkedList(context);
    { // Prevent name conflicts
        context ctx = { .offset = 0, .pref = start, .curr = start };
        list_push_back(stack, &ctx);
    }
    while (!list_empty(stack))
    {
        context *ctx = list_pop_value(stack);
        visited[ctx->curr->id] = 1;
        // No transition need to be added from terminal states:
        // a substring match can't fail once it has reached a terminal state.
        if (ctx->curr->terminal)
            continue;

        for (size_t i = 0; i < aut->transition_table->width; i++)
        {
            Letter letter = letter_mapping[i];
            LinkedList *pref_list =
                matrix_get(aut->transition_table, i, ctx->pref->id);
            LinkedList *curr_list =
                matrix_get(aut->transition_table, i, ctx->curr->id);

            if (!list_empty(pref_list))
            {
                State *pref_dest = *(State **)pref_list->next->data;

                // If there is a transition in the prefix but not
                // in the current position, add it at the current position
                if (list_empty(curr_list))
                {
                    automaton_add_transition(aut, ctx->curr, pref_dest, letter,
                                             0);
                    LinkedList *tr =
                        get_matrix_elt(aut, ctx->curr->id, letter, 0);
                    tr->data = (void *)(ctx->offset + 1);
                }
                else
                {
                    State *curr_dest = *(State **)curr_list->next->data;
                    if (visited[curr_dest->id])
                        continue;
                    int fake = pref_dest->id == curr_dest->id;
                    pref_dest = fake ? start : pref_dest;
                    context c = { .offset = ctx->offset + !fake,
                                  .pref = pref_dest,
                                  .curr = curr_dest };
                    list_push_back(stack, &c);
                }
            }
            else if (!list_empty(curr_list))
            {
                State *curr_dest = *(State **)curr_list->next->data;
                if (visited[curr_dest->id])
                    continue;
                context c = { .offset = 0, .pref = start, .curr = curr_dest };
                list_push_back(stack, &c);
            }
        }
    }

    list_free(stack);

    return aut;
}

static void free_powersets(Map *powersets)
{
    map_foreach_key(Set *, set, powersets, map_free(set);) map_free(powersets);
}
