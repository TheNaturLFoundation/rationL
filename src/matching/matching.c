#include "matching/matching.h"

#include <printf.h>
#include <string.h>

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
    Array *matches = Array(Match *);
    const char *string_start = string;
    for (; *string != 0; string++)
    {
        arr_foreach(State *, start, automaton->starting_states)
        {
            char *end = submatch_nfa_from_state(automaton, string, start);
            if (end != NULL)
            {
                Match *match = SAFEMALLOC(sizeof(Match));
                match->string = string_start;
                match->start = string - string_start;
                match->length = end - string;
                // TODO: Fix when groups are supported
                match->nb_groups = 0;
                match->groups = NULL;
                array_append(matches, &match);
                string = end - 1;
                break;
            }
        }
    }

    return matches;
}

Array *search_dfa(const Automaton *automaton, const char *string)
{
    typedef struct context
    {
        State *current_state;
        const char *current_str;
        LinkedList *queue_top;
    } context;

    // The queue can contain two types of integers:
    // * If it is positive, it is simply a letter.
    // * If it is negative, it is a marker.
    //   - If the marker is odd (-2n), it indicates an entering
    //     transition into the group n.
    //   - If it is even (-2n - 1), it indicates of leaving
    //      transition from the group n.
    LinkedList *queue = LinkedList(int);
    Array *matches = Array(Match *);

    context *last_final = NULL;
    context ctx;
    State *start = *(State **)array_get(automaton->starting_states, 0);
    State *curr = start;
    const char *string_start = string; // This one won't move
    const char *match_start = string;
    while (*string != 0)
    {
        LinkedList *tr = get_matrix_elt(automaton, curr->id, *string, 0);
        if (tr == NULL)
        {
            if (last_final != NULL)
            {
                string = last_final->current_str;
                curr = last_final->current_state;

                Match *match = SAFEMALLOC(sizeof(Match));
                match->string = string_start;
                match->start = match_start - string_start;
                match->length = string - match_start;
                match->nb_groups = automaton->nb_groups;
                if (match->nb_groups == 0)
                    match->groups = NULL;
                else
                    match->groups =
                        SAFECALLOC(sizeof(char *), match->nb_groups);

                Array **arrays = SAFECALLOC(sizeof(Array *), automaton->nb_groups);
                for (LinkedList *list_elt = queue->next;
                     list_elt != last_final->queue_top->next && list_elt != NULL;
                     list_elt = list_elt->next)
                {
                    int *value = list_elt->data;
                    if (*value > 0)
                    {
                        for (size_t i = 0; i < automaton->nb_groups; i++)
                            if (arrays[i] != NULL)
                                array_append(arrays[i], value);
                    }
                    else
                    {
                        int mark = -*value;
                        int group = mark / 2;
                        if (mark % 2 == 0)
                            arrays[group] = Array(char);
                        else
                        {
                            char end_char = 0;
                            array_append(arrays[group], &end_char);
                            match->groups[group] = arrays[group]->data;
                            match->groups[group] = SAFEREALLOC(
                                match->groups[group], arrays[group]->size);
                            free(arrays[group]);
                            arrays[group] = NULL;
                        }
                    }
                }
                free(arrays);

                last_final = NULL;
                array_append(matches, &match);
            }
            else
            {
                if (curr == start)
                    string++;
                else
                    curr = start;
                match_start = string;
            }

            continue;
        }
        string++;
        State *prev_curr = curr;
        curr = *(State **)tr->next->data;
        // Adding elements to the queue
        Set *leaving = get_leaving_group((Automaton *)automaton,
                                         prev_curr, curr, *(string - 1), 0);
        if (leaving != NULL)
        {
            map_foreach_key(size_t, group, leaving, {
                int mark = -(2 * group + 1);
                list_push_back(queue, &mark);
            })
        }
        leaving = get_leaving_group((Automaton *)automaton,
                                    prev_curr, NULL, *(string - 1), 0);

        if (leaving != NULL)
        {
            map_foreach_key(size_t, group, leaving, {
                int mark = -(2 * group + 1);
                list_push_back(queue, &mark);
            })
        }
        Set *entering = get_entering_groups((Automaton *)automaton, NULL,
                                            prev_curr, *(string - 1), 0);
        if (entering != NULL)
        {
            map_foreach_key(size_t, group, entering, {
                int mark = -2 * group;
                list_push_back(queue, &mark);
            })
        }

        {
            int mark = (unsigned char)*(string - 1);
            list_push_back(queue, &mark);
        }
        entering = get_entering_groups((Automaton *)automaton,
                                            prev_curr, curr, *(string - 1), 0);
        if (entering != NULL)
        {
            map_foreach_key(size_t, group, entering, {
                int mark = -2 * group;
                list_push_back(queue, &mark);
            })
        }

        if (tr->data != NULL)
        {
            // If a final state was encountered, don't take a back transition
            // that would move the start of the match
            if (last_final != NULL)
            {
                string = last_final->current_str;
                curr = last_final->current_state;

                Match *match = SAFEMALLOC(sizeof(Match));
                match->string = string_start;
                match->start = match_start - string_start;
                match->length = string - match_start;
                match->nb_groups = automaton->nb_groups;
                if (match->nb_groups == 0)
                    match->groups = NULL;
                else
                    match->groups =
                        SAFECALLOC(sizeof(char *), match->nb_groups);

                Array **arrays = SAFECALLOC(sizeof(Array *), automaton->nb_groups);
                for (LinkedList *list_elt = queue->next;
                     list_elt != last_final->queue_top->next && list_elt != NULL;
                     list_elt = list_elt->next)
                {
                    int *value = list_elt->data;
                    if (*value > 0)
                    {
                        for (size_t i = 0; i < automaton->nb_groups; i++)
                            if (arrays[i] != NULL)
                                array_append(arrays[i], value);
                    }
                    else
                    {
                        int mark = -*value;
                        int group = mark / 2;
                        if (mark % 2 == 0)
                            arrays[group] = Array(char);
                        else
                        {
                            char end_char = 0;
                            array_append(arrays[group], &end_char);
                            match->groups[group] = arrays[group]->data;
                            match->groups[group] = SAFEREALLOC(
                                match->groups[group], arrays[group]->size);
                            // Don't use array_free since the data field
                            // is used elsewhere
                            free(arrays[group]);
                            arrays[group] = NULL;
                        }
                    }
                }
                free(arrays);

                last_final = NULL;
                array_append(matches, &match);
                continue;
            }
            size_t offset = (size_t)tr->data;
            match_start = string - offset;
        }

        if (curr->terminal)
        {
            Set *groups = get_leaving_group((Automaton *)automaton, curr, NULL, 0, 0);
            if (groups != NULL)
            {
                map_foreach_key(size_t, group, groups, {
                    int mark = -(2 * group + 1);
                    list_push_back(queue, &mark);
                })
            }
            ctx.current_state = curr;
            ctx.current_str = string;
            ctx.queue_top = queue;
            while (ctx.queue_top->next != NULL)
                ctx.queue_top = ctx.queue_top->next;
            last_final = &ctx;
        }
    }

    if (last_final != NULL)
    {
        string = last_final->current_str;
        Match *match = SAFEMALLOC(sizeof(Match));
        match->string = string_start;
        match->start = match_start - string_start;
        match->length = string - match_start;
        match->nb_groups = automaton->nb_groups;
        if (match->nb_groups == 0)
            match->groups = NULL;
        else
            match->groups =
                SAFECALLOC(sizeof(char *), match->nb_groups);

        Array **arrays = SAFECALLOC(sizeof(Array *), automaton->nb_groups);
        for (LinkedList *list_elt = queue->next;
             list_elt != last_final->queue_top->next && list_elt != NULL;
             list_elt = list_elt->next)
        {
            int *value = list_elt->data;
            if (*value > 0)
            {
                for (size_t i = 0; i < automaton->nb_groups; i++)
                    if (arrays[i] != NULL)
                        array_append(arrays[i], value);
            }
            else
            {
                int mark = -*value;
                int group = mark / 2;
                if (mark % 2 == 0)
                    arrays[group] = Array(char);
                else
                {
                    char end_char = 0;
                    array_append(arrays[group], &end_char);
                    match->groups[group] = arrays[group]->data;
                    match->groups[group] = SAFEREALLOC(
                        match->groups[group], arrays[group]->size);
                    free(arrays[group]);
                    arrays[group] = NULL;
                }
            }
        }
        free(arrays);

        last_final = NULL;
        array_append(matches, &match);
    }

    return matches;
}

char *replace_nfa(const Automaton *automaton, const char *string,
                  const char *replace)
{
    typedef struct
    {
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
    char *result = final; // Keep a pointer to the head of the string
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

        list_free_from(curr_state_list);
        list_free_from(curr_str_list);

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
            get_matrix_elt(automaton, curr_state->id, 0, 1);
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
            transition =
                get_matrix_elt(automaton, curr_state->id, *curr_str, 0);
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

void free_match(Match *match)
{
    if (match != NULL && match->groups != NULL)
    {
        for (size_t i = 0; i < match->nb_groups; i++)
            free(match->groups[i]);
        free(match->groups);
    }
    free(match);
}
