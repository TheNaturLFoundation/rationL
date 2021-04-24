#include "automaton/thompson.h"

#include "datatypes/array.h"
#include "datatypes/linked_list.h"
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
    for (size_t i = 0; i < b->size; i++)
        for (size_t j = 0; j < NUMBER_OF_SYMB; j++)
        {
            if(b->lookup_table[j] != -1)
            {
                LinkedList *targets = matrix_get(b->transition_table,
                    b->lookup_table[j], i);
                if (targets != NULL)
                {
                    State *src = *(State **)array_get(states_b_htab, i);
                    list_foreach(State *, dst, targets)
                    {
                        State *real_dst =
                            *(State **)array_get(states_b_htab, dst->id);
                        automaton_add_transition(a, src, real_dst, j, j == EPSILON_INDEX);
                    }
                }
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

State *add_new_entry(Automaton *a)
{
    State *new_entry = State(0);
    automaton_add_state(a, new_entry, 0);
    arr_foreach(State *, entry, a->starting_states)
    {
       automaton_add_transition(a, new_entry, entry, 'e', 1);
    }
    array_clear(a->starting_states);
    array_append(a->starting_states, &new_entry);
    return new_entry;
}

void unite(Automaton *a, Automaton *b)
{
    Array *states_b_htab = connect_automatons(a, b, 0);
    add_new_entry(a);
    State *new_end = State(0);
    automaton_add_state(a, new_end, 0);
    arr_foreach(State *, state, a->states)
    {
        if (state->terminal)
        {
            automaton_add_transition(a, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    new_end->terminal = 1;
    array_free(states_b_htab);
}

void kleene(Automaton *a)
{
    State *new_end = State(0);
    State *new_entry = State(0);
    automaton_add_state(a, new_entry, 0);
    automaton_add_state(a, new_end, 0);
    automaton_add_transition(a, new_entry, new_end, 'e', 1);
    arr_foreach(State *, state, a->states)
    {
        if (state->terminal)
        {
            arr_foreach(State *, entry, a->starting_states)
            {
                automaton_add_transition(a, state, entry, 'e', 1);
            }
            automaton_add_transition(a, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    arr_foreach(State *, entries, a->starting_states)
    {
        automaton_add_transition(a, new_entry, entries, 'e', 1);
    }
    array_clear(a->starting_states);
    array_append(a->starting_states, &new_entry);
    new_end->terminal = 1;
}

void maybe(Automaton *aut)
{
    State *new_entry = add_new_entry(aut);
    State *new_end = State(0);
    automaton_add_state(aut, new_end, 0);
    arr_foreach(State *, state, aut->states)
    {
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    new_end->terminal = 1;
    automaton_add_transition(aut, new_entry, new_end, 'e', 1);
}

void exists(Automaton *aut)
{
    State *new_end = State(0);
    automaton_add_state(aut, new_end, 0);
    arr_foreach(State *, state, aut->states)
    {
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    arr_foreach(State *, starting_state, aut->starting_states)
    {
        automaton_add_transition(aut, new_end, starting_state, 'e', 1);
    }
    new_end->terminal = 1;
}

Automaton *thompson(BinTree *tree)
{
    if(tree == NULL)
        return NULL;
    if (tree->left == NULL && tree->right == NULL)
    {
        State *entry_state = State(0);
        entry_state->id = 0;
        State *letter_state = State(1);
        letter_state->id = 1;
        Automaton *aut = Automaton(2, 2);
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
    case KLEENE_STAR: {
        Automaton *child = thompson(tree->left);
        kleene(child);
        return child;
    }
    case MAYBE: {
        Automaton *child = thompson(tree->left);
        maybe(child);
        return child;
    }
    case EXISTS: {
        Automaton *child = thompson(tree->left);
        exists(child);
        return child;
    }
    }

    return NULL; //LCOV_EXCL_LINE
}
