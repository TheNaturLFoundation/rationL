#include <string.h>
#include "matching/matching.h"
#include "utils/memory_utils.h"
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
/*
 * Useless now, replaced by submatch_nfa_from_state
 * I keep it commented in case I need it again later
static int match_nfa_from_state(const Automaton *automaton, const char *string,
                         State *start)
{
    struct LinkedList *state_queue = LinkedList(size_t);
    struct LinkedList *str_queue = LinkedList(char *);
    list_push_back(state_queue, &start);
    list_push_back(str_queue, &string);
    while (!list_empty(state_queue))
    {
        // Get the current id
        LinkedList *curr_state_list = list_pop_front(state_queue);
        State *curr_state = *(State **)curr_state_list->data;
        // Get the current string
        LinkedList *curr_str_list = list_pop_front(str_queue);
        char *curr_str = *(char **)curr_str_list->data;

        list_free(curr_state_list);
        list_free(curr_str_list);

        if (*curr_str == 0 && curr_state->terminal)
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
            State *state = *(State **)transition->data;
            list_push_back(state_queue, &state);
            // An epsilon-transition doesn't increment the pointer on the string
            list_push_back(str_queue, &curr_str);
        }

        // Test the current letter
        if (*curr_str != 0)
        {
            transition = matrix_get(automaton->transition_table, *curr_str,
                                    curr_state->id)
                             ->next;
            for (; transition != NULL; transition = transition->next)
            {
                State *state = *(State **)transition->data;
                list_push_back(state_queue, &state);
                char *next_str = curr_str + 1;
                list_push_back(str_queue, &next_str);
            }
        }
    }
    list_free(state_queue);
    list_free(str_queue);
    return 0;
}
 */

static char *submatch_nfa_from_state(const Automaton *, const char *, State *);

Match *match_nfa(const Automaton *automaton, const char *string)
{
    arr_foreach(State *, start, automaton->starting_states)
    {
        char *end = submatch_nfa_from_state(automaton, string, start);
        if (end != NULL)
        {
            Match *match = SAFEMALLOC(sizeof(Match));
            match->string = string;
            match->start = 0;
            match->length = end - string;

            // TODO: Fix when groups are supported
            match->nb_groups = 0;
            match->groups = NULL;

            return match;
        }
    }
    return NULL;
}

Array *search_nfa(const Automaton *automaton, const char *string)
{
    Array *substrings = Array(char *);
    for (; *string != 0; string++)
    {
        arr_foreach(State *, start, automaton->starting_states)
        {
            char *end = submatch_nfa_from_state(automaton, string, start);
            if (end != NULL)
            {
                size_t length = end - string;
                char *copy = SAFECALLOC(length + 1, sizeof(char));
                memcpy(copy, string, length);
                // Add the string to the array
                array_append(substrings, &copy);
                string = end - 1;
                break;
            }
        }
    }
    return substrings;
}


char *replace_nfa(const Automaton *automaton, const char *string,
                   const char *replace)
{
    typedef struct {
        size_t start;
        size_t end;
    } match_bounds;
    Array *matches = Array(match_bounds);

    size_t final_size = strlen(string);
    size_t repl_size = strlen(replace);
    for (const char *curr = string; *curr != 0; curr++)
    {
        arr_foreach(State *, start, automaton->starting_states)
        {
            char *end = submatch_nfa_from_state(automaton, curr, start);
            if (end != NULL)
            {
                size_t len = end - curr;
                final_size = final_size - len + repl_size;

                match_bounds bounds;
                bounds.start = curr - string;
                bounds.end = end - string - 1;
                array_append(matches, &bounds);
                curr = string + bounds.end;
                break;
            }
        }
    }

    const char *start = string;
    char *final = SAFECALLOC(final_size + 1, sizeof(char));
    char *result = final;  // Keep a pointer to the head of the string
    size_t last_bound = 0;
    arr_foreach(match_bounds, bounds, matches)
    {
        memcpy(final, string, bounds.start - last_bound);
        final += bounds.start - last_bound;
        last_bound = bounds.end + 1;
        strcpy(final, replace);
        final += repl_size;
        string = start + last_bound;
    }
    // Add the remaining characters that do not need to be changed
    for (; *string != 0; string++, final++)
        *final = *string;

    array_free(matches);

    return result;
}

/**
 * Test whether a string contains a substring recognized by a suffix of a NFA.
 * @param automaton Some NFA
 * @param string The string to test
 * @param start The state from which the search starts
 * @return The end pointer when the match ends if there is a match, else NULL.
 */
static char *submatch_nfa_from_state(const Automaton *automaton,
                                        const char *string, State *start)
{
    char *max_str = NULL;

    struct LinkedList *state_queue = LinkedList(size_t);
    struct LinkedList *str_queue = LinkedList(char *);
    list_push_back(state_queue, &start);
    list_push_back(str_queue, &string);
    while (!list_empty(state_queue))
    {
        // Get the current id
        LinkedList *curr_state_list = list_pop_front(state_queue);
        State *curr_state = *(State **)curr_state_list->data;
        // Get the current string
        LinkedList *curr_str_list = list_pop_front(str_queue);
        char *curr_str = *(char **)curr_str_list->data;

        list_free(curr_state_list);
        list_free(curr_str_list);

        if (curr_state->terminal)
        {
            if (curr_str > max_str)
                max_str = curr_str;
            if (*curr_str == 0)
            {
                list_free(state_queue);
                list_free(str_queue);
                return max_str;
            }
        }

        // Test epsilon transitions
        LinkedList *transition =
            get_matrix_elt(automaton, curr_state->id, 0, 0);
        if (transition != NULL)
            transition = transition->next;
        for (; transition != NULL; transition = transition->next)
        {
            State *state = *(State **)transition->data;
            list_push_back(state_queue, &state);
            // An epsilon-transition doesn't increment the pointer on the string
            list_push_back(str_queue, &curr_str);
        }

        // Test the current letter
        if (*curr_str != 0)
        {
            transition = get_matrix_elt(automaton, curr_state->id, *curr_str, 0);
            if (transition != NULL)
                transition = transition->next;
            for (; transition != NULL; transition = transition->next)
            {
                State *state = *(State **)transition->data;
                list_push_back(state_queue, &state);
                char *next_str = curr_str + 1;
                list_push_back(str_queue, &next_str);
            }
        }
    }
    list_free(state_queue);
    list_free(str_queue);
    return max_str;
}
