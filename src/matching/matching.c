#include "matching.h"

#include <printf.h>

// TODO: When it works, remove recursion
int match_nfa_from_state(const Automaton *automaton, const char *string,
                                State *start)
{
    if (start->terminal && *string == 0)
        return 1;

    LinkedList **sentinel = array_get(automaton->adj_lists, start->id);
    for (LinkedList *current = (*sentinel)->next; current != NULL;
         current = current->next)
    {
        Transition *transition = current->data;
        if (transition->is_epsilon || transition->value == *string)
        {
            if (match_nfa_from_state(automaton,
                                     string + !transition->is_epsilon,
                                     transition->target))
                return 1;
        }
    }
    return 0;
}

int match_nfa(const Automaton *automaton, const char *string)
{
    arr_foreach(State *, start, automaton->starting_states)
        if (match_nfa_from_state(automaton, string, start))
            return 1;
    return 0;
}
