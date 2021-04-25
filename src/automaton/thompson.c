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
    State *new_entry = State(0);
    automaton_add_state(aut, new_entry, 0);
    State *new_end = State(1);
    automaton_add_state(aut, new_end, 0);
    arr_foreach(State *, entry_state, aut->starting_states)
    {
        automaton_add_transition(aut, new_entry, entry_state, 'e', 1);
    }
    arr_foreach(State *, state, aut->states)
    {
        if (state->terminal && state != new_end)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
        }
    }
    array_clear(aut->starting_states);
    array_append(aut->starting_states, &new_entry);
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
        thompson_recur(tree->right, aut);
        thompson_recur(tree->left, aut);
        unite(aut);
        break;
    }
    case KLEENE_STAR: {
    }
    case MAYBE: {
    }
    case EXISTS: {
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
