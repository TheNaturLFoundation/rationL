#include "automaton/thompson.h"

#include "automaton/automaton.h"
#include "datatypes/array.h"
#include "datatypes/linked_list.h"
#include "parsing/lexer.h"
#include "parsing/parsing.h"

void concatenate(Automaton *aut)
{
    State *entry_to_patch = *(State **)array_get(
        aut->starting_states, aut->starting_states->size - 2);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *end_to_patch = *(State **)array_get(aut->states, i);
        if (end_to_patch->terminal)
        {
            automaton_add_transition(aut, end_to_patch, entry_to_patch, 'e', 1);
            end_to_patch->terminal = 0;
            break;
        }
    }
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
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            if (count == 1)
                break;
            else
                count++;
        }
    }
    automaton_add_transition(aut, new_start, first_start, 'e', 1);
    automaton_add_transition(aut, new_start, second_start, 'e', 1);
    array_remove(aut->starting_states, aut->starting_states->size - 1);
    array_set(aut->starting_states, aut->starting_states->size - 1, &new_start);
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
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
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
    State *current_start = *(State **)array_get(aut->starting_states,
                                                aut->starting_states->size - 1);
    automaton_add_state(aut, new_end, 0);
    automaton_add_transition(aut, new_end, current_start, 'e', 1);
    for (int i = aut->states->size - 1; i >= 0; i--)
    {
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            break;
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
        State *state = *(State **)array_get(aut->states, i);
        if (state->terminal)
        {
            automaton_add_transition(aut, state, new_end, 'e', 1);
            state->terminal = 0;
            break;
        }
    }
    automaton_add_transition(aut, new_start, start, 'e', 1);
    array_set(aut->starting_states, aut->starting_states->size - 1, &new_start);
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
        Symbol sym = *(Symbol *)tree->data;
        if (sym.type == LETTER)
        {
            automaton_add_transition(aut, entry_state, letter_state,
                                     sym.value.letter, 0);
        }
        else
        {
            arr_foreach(Letter, c, sym.value.letters)
            {
                automaton_add_transition(aut, entry_state, letter_state,
                                         c, 0);
            }
        }
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

void count_symbols(BinTree *tree, size_t *size, size_t *letter_count)
{
    if (tree->left == NULL && tree->right == NULL)
    {
        Symbol sym = *(Symbol*)tree->data;
        if(sym.type == LETTER)
            *letter_count += 1;
        else
            *letter_count += sym.value.letters->size;
        *size += 1;
    }
    else
    {
        if (((Symbol *)tree->data)->value.operator!= CONCATENATION)
            *size += 1;
    }
    if (tree->left)
        count_symbols(tree->left, size, letter_count);
    if (tree->right)
        count_symbols(tree->right, size, letter_count);
}

Automaton *thompson(BinTree *tree)
{
    if (tree == NULL)
        return NULL;
    size_t size = 0;
    size_t letter_count = 0;
    count_symbols(tree, &size, &letter_count);
    Automaton *aut = Automaton(2 * size,
                               letter_count + 1 > 256 ? 256 : letter_count + 1);
    thompson_recur(tree, aut);
    return aut;
}
