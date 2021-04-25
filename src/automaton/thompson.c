#include "automaton/thompson.h"

#include "automaton/automaton.h"
#include "datatypes/array.h"
#include "datatypes/linked_list.h"
#include "parsing/parsing.h"

void concatenate(Automaton *aut)
{
    State *entry_to_patch = *(State **)array_get(
        aut->starting_states, aut->starting_states->size - 2);
    State *end_to_patch =
        *(State **)array_get(aut->states, aut->states->size - 1);
    automaton_add_transition(aut, end_to_patch, entry_to_patch, 'e', 1);
    end_to_patch->terminal = 0;
    array_remove(aut->starting_states, aut->starting_states->size - 2);
}

void unite(Automaton *aut)
{
    State *new_start = State(0);
    State *new_end = State(0);
    State *first_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    State *second_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 2);

    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    int count = 0;
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State* state = *(State**) array_get(aut->states, i);
        if(state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            if(count == 1)
                break;
            else
                count++;
        }
    }
    automaton_add_transition(aut, new_start, first_start, 'e', 1);
    automaton_add_transition(aut, new_start, second_start, 'e', 1);
    array_remove(aut->starting_states, aut->starting_states->size-1);
    array_set(aut->starting_states, aut->starting_states->size-1, &new_start);
    new_end->terminal = 1;
}

void kleene(Automaton *aut)
{
    State *new_start = State(0);
    State *new_end = State(0);
    State *current_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State* state = *(State**) array_get(aut->states, i);
        if(state->terminal)
        {
            automaton_add_transition(aut, state, current_start, 'e', 1);
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            break;
        }
    }
    automaton_add_transition(aut, new_start, current_start, 'e', 1);
    automaton_add_transition(aut, new_start, new_end, 'e', 1);
    array_set(aut->starting_states, aut->starting_states->size - 1, &new_start);
    new_end->terminal = 1;
}

void exists(Automaton *aut)
{
    State *new_end = State(0);
    State *curr_start = *(State **)array_get(aut->starting_states, 0);
    automaton_add_state(aut, new_end, 0);
    automaton_add_transition(aut, new_end, curr_start, 'e', 1);
    arr_foreach(State *, state, aut->states)
    {
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    new_end->terminal = 1;
}

void maybe(Automaton *aut)
{
    State *new_start = State(0);
    State *new_end = State(0);
    State *start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    automaton_add_state(aut, new_start, 0);
    automaton_add_state(aut, new_end, 0);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State* state = *(State**) array_get(aut->states, i);
        if(state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            break;
        }
    }
    automaton_add_transition(aut, new_start, start, 'e', 1);
    array_set(aut->starting_states, aut->starting_states->size-1, &new_start);
    new_end->terminal = 1;
    automaton_add_transition(aut, new_start, new_end, 'e', 1);
}

void thompson_recur(BinTree *tree, Automaton *aut)
{
    if (tree->left == NULL && tree->right == NULL)
    {
        State *entry_state = State(0);
        State *letter_state = State(1);
        automaton_add_state(aut, entry_state, 1);
        automaton_add_state(aut, letter_state, 0);
        automaton_add_transition(aut, entry_state, letter_state,
                                 ((Symbol *)(tree->data))->value.letter, 0);
        return;
    }

    Operator operator=((Symbol *)tree->data)->value.operator;

    switch (operator)
    {
    case CONCATENATION: {
        thompson_recur(tree->right, aut);
        thompson_recur(tree->left, aut);
        concatenate(aut);
        break;
    }
    case UNION: {
        thompson_recur(tree->left, aut);
        thompson_recur(tree->right, aut);
        unite(aut);
        break;
    }
    case KLEENE_STAR: {
        thompson_recur(tree->left, aut);
        kleene(aut);
        break;
    }
    case MAYBE: {
        thompson_recur(tree->left, aut);
        maybe(aut);
        break;
    }
    case EXISTS: {
        thompson_recur(tree->left, aut);
        exists(aut);
        break;
    }
    }
}

Automaton *thompson(BinTree *tree)
{
    if (tree == NULL)
        return NULL;
    Automaton *aut = Automaton(200, 255);
    thompson_recur(tree, aut);
    return aut;
}
