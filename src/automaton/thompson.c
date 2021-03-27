#include "automaton/thompson.h"

#include "automaton/automaton.h"
#include "datatypes/array.h"
#include "parsing/parsing.h"

int is_state_entry(Automaton *aut, State *state)
{
    arr_foreach(State *, entry_state, aut->starting_states)
    {
        if (state == entry_state)
            return 1;
    }
    return 0;
}

// Connects two automatons and returns a correspondance
// table between ids of b and the new state in a
Array *connect_automatons(Automaton *a, Automaton *b, int remap_entries)
{
    Array *states_b_htab = Array(State *);
    arr_foreach(State *, state_b, b->states)
    {
        State *state = State(state_b->terminal);
        state->id = a->size;
        array_append(states_b_htab, &state);
        automaton_add_state(a, state,
                            remap_entries ? 0 : is_state_entry(b, state_b));
    }
    size_t index = 0;
    arr_foreach(LinkedList *, transition_b, b->adj_lists)
    {
        transition_b = transition_b->next;
        while (transition_b != NULL)
        {
            Transition *transition = transition_b->data;
            State *src = *(State **)array_get(states_b_htab, index);
            State *dst =
                *(State **)array_get(states_b_htab, transition->target->id);
            automaton_add_transition(a, src, dst, transition->value,
                                     transition->is_epsilon);
            index += 1;
            transition_b = transition_b->next;
        }
    }
    return states_b_htab;
}

void concatenate(Automaton *a, Automaton *b)
{
    Array *terminal_states = Array(State *);
    arr_foreach(State *, state_a, a->states)
    {
        if (state_a->terminal)
        {
            array_append(terminal_states, &state_a);
            state_a->terminal = 0;
        }
    }
    Array *states_b_htab = connect_automatons(a, b, 1);
    arr_foreach(State *, a_states, terminal_states)
    {
        arr_foreach(State *, entry_state, b->starting_states)
        {
            State *dst = *(State **)array_get(states_b_htab, entry_state->id);
            automaton_add_transition(a, a_states, dst, 'e', 1);
        }
    }
    array_free(terminal_states);
    array_free(states_b_htab);
}

void unite(Automaton *a, Automaton *b)
{
    Array *states_b_htab = connect_automatons(a, b, 0);
    State *new_entry = State(0);
    automaton_add_state(a, new_entry, 0);
    arr_foreach(State*, entry, a->starting_states)
    {
        automaton_add_transition(a, new_entry, entry, 'e', 1);
    }
    array_clear(a->starting_states);
    array_append(a->starting_states, &new_entry);
    array_free(states_b_htab);
}

Automaton *thompson(BinTree *tree)
{
    if (tree->left == NULL && tree->right == NULL)
    {
        State *entry_state = State(0);
        entry_state->id = 0;
        State *letter_state = State(1);
        letter_state->id = 1;
        Automaton *aut = Automaton();
        automaton_add_state(aut, entry_state, 1);
        automaton_add_state(aut, letter_state, 0);
        automaton_add_transition(aut, entry_state, letter_state,
                                 ((Symbol *)(tree->data))->value.letter, 0);
        return aut;
    }

    Operator operator=((Symbol *)tree->data)->value.operator;

    switch (operator)
    {
    case CONCATENATION: {
        Automaton *left = thompson(tree->left);
        Automaton *right = thompson(tree->right);
        concatenate(left, right);
        automaton_free(right);
        return left;
    }
    case UNION: {
        Automaton *left = thompson(tree->left);
        Automaton *right = thompson(tree->right);
        unite(left, right);
        automaton_free(right);
        return left;
    }
    default:
        break;
    }

    return NULL;
}
