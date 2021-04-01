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
        // The three following lines pop the first element of the list since list_pop_front crashes
        LinkedList *curr_state_list = state_queue->next;
        state_queue->next = curr_state_list->next;
        curr_state_list->next = NULL;
        // LinkedList *curr_state_list = list_pop_front(state_queue); (crashes :/)
        State *curr_state = *(State **)curr_state_list->data;
        list_free(curr_state_list);
        // Get the current string
        // same probleme here
        LinkedList *curr_str_list = str_queue->next;
        str_queue->next = curr_str_list->next;
        curr_str_list->next = NULL;
        // LinkedList *curr_str_list = list_pop_front(str_queue);
        char *curr_str = *(char **)curr_str_list->data;
        list_free(curr_str_list);

        if (curr_state->terminal && *curr_str == 0)
            return 1;

        LinkedList **sentinel = array_get(automaton->adj_lists, curr_state->id);
        for (struct LinkedList *succ = (*sentinel)->next; succ != NULL;
             succ = succ->next)
        {
            Transition *transition = succ->data;
            if (transition->is_epsilon)
            {
                list_push_back(state_queue, &transition->target);
                list_push_back(str_queue, &curr_str);
            }
            else if (transition->value == *curr_str)
            {
                list_push_back(state_queue, &transition->target);
                char *next_str = curr_str + 1;
                list_push_back(str_queue, &next_str);
            }
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
