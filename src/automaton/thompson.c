#include "automaton/thompson.h"

#include "automaton/automaton.h"
#include "datatypes/array.h"
#include "parsing/parsing.h"


void concatenate(Automaton *a, Automaton *b)
{
    (void)a;
    (void)b;
}


void unite(Automaton *a, Automaton *b)
{
    (void)a;
    (void)b;
}

void kleene(Automaton *a)
{
    (void)a;
}

void maybe(Automaton* aut)
{
    (void)aut;
}

void exists(Automaton* aut)
{
    (void)aut;
}

Automaton *thompson(BinTree *tree)
{
    if (tree->left == NULL && tree->right == NULL)
    {
        State *entry_state = State(0);
        entry_state->id = 0;
        State *letter_state = State(1);
        letter_state->id = 1;
        Automaton *aut = Automaton(2);
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
    default:
        break;
    }

    return NULL;
}
