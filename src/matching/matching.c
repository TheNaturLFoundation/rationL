#include "matching.h"

#include <printf.h>

/*
 * Recursive function that has been replaced by an iterative one (see below)
int match_nfa_from_state_rec(const Automaton *automaton, const char *string,
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
            if (match_nfa_from_state_rec(automaton,
                                     string + !transition->is_epsilon,
                                     transition->target))
                return 1;
        }
    }
    return 0;
}
 */

int match_nfa_from_state(const Automaton *automaton, const char *string,
                         State *start)
{
    struct LinkedList *state_queue = LinkedList(size_t);
    struct LinkedList *str_queue = LinkedList(char *);
    list_push_back(state_queue, &start);
    list_push_back(str_queue, &string);
    while (state_queue->next != NULL)
    {
        // Get the current id
        LinkedList *curr_state_list = list_pop_front(state_queue);
        State *curr_state = *(State **)curr_state_list->data;
        // Get the current string
        LinkedList *curr_str_list = list_pop_front(str_queue);
        char *curr_str = *(char **)curr_str_list->data;

        list_free(curr_state_list);
        list_free(curr_str_list);

        if (curr_state->terminal && *curr_str == 0)
        {
            list_free(state_queue);
            list_free(str_queue);
            return 1;
        }

        // Test transition transitions
        LinkedList *transition =
            matrix_get(automaton->transition_table, 0, curr_state->id)->next;
        for (; transition != NULL; transition = transition->next)
        {
            State *state = transition->data;
            list_push_back(state_queue, &state);
            // An epsilon-transition doesn't increment the pointer on the string
            list_push_back(str_queue, &curr_str);
        }
        // Test the current letter
        transition = matrix_get(automaton->transition_table,
                                *curr_str, curr_state->id)->next;
        for (; transition != NULL; transition = transition->next)
        {
            State *state = transition->data;
            list_push_back(state_queue, &state);
            char *next_str = curr_str + 1;
            list_push_back(str_queue, &next_str);
        }
    }
    list_free(state_queue);
    list_free(str_queue);
    return 0;
}

int match_nfa(const Automaton *automaton, const char *string)
{
    arr_foreach(State *, start, automaton->starting_states)
    {
        if (match_nfa_from_state(automaton, string, start))
            return 1;
    }
    return 0;
}
