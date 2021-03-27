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

Automaton *concatenate(Automaton *a, Automaton *b)
{
    Automaton *result = Automaton();
    arr_foreach(State *, state_a, a->states)
    {
        State *state = State(0);
        automaton_add_state(result, state, is_state_entry(a, state_a));
    }
    size_t index = 0;
    arr_foreach(LinkedList *, transitions, a->adj_lists)
    {
        transitions = transitions->next;
        while (transitions != NULL)
        {
            Transition *transition = transitions->data;
            automaton_add_transition(
                result, *(State **)array_get(result->states, index),
                *(State **)array_get(result->states, transition->target->id),
                transition->value, 0);
            transitions = transitions->next;
        }

        index += 1;
    }

    Array *states_b_tab = Array(State *);

    arr_foreach(State *, state_b, b->states)
    {
        State *state = State(state_b->terminal);
        state->id = result->size;
        array_append(states_b_tab, &state);
        automaton_add_state(result, state, 0);
    }
    index = 0;
    arr_foreach(LinkedList *, transition_b, b->adj_lists)
    {
        transition_b = transition_b->next;
        while (transition_b != NULL)
        {
            Transition *transition = transition_b->data;
            State *src = *(State**)array_get(states_b_tab, index);
            State *dst = *(State**)array_get(states_b_tab, transition->target->id);
            automaton_add_transition(result, src, dst, transition->value,
                                     transition->is_epsilon);
            index += 1;
            transition_b = transition_b->next;
        }
    }
    arr_foreach(State*, a_states, a->states)
    {
        if(a_states->terminal)
        {
            arr_foreach(State*, entry_state, b->starting_states)
            {
                State* dst = *(State**)array_get(states_b_tab, entry_state->id);
                automaton_add_transition(result, *(State**)array_get(result->states, a_states->id), dst, 'e', 1);
            }
        }
    }
    array_free(states_b_tab);
    return result;
}

Automaton *recur_thompson(BinTree *tree)
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
        Automaton *left = recur_thompson(tree->left);
        Automaton *right = recur_thompson(tree->right);
        Automaton *result = concatenate(left, right);
        automaton_free(left);
        automaton_free(right);
        return result;
    }
    default:
        break;
    }

    return NULL;
}

Automaton *thompson(BinTree *tree)
{
    return recur_thompson(tree);
}
