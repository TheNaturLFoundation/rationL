#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "automaton/automaton.h"
#include "datatypes/bin_tree.h"
#include "utils/memory_utils.h"

Automaton *automaton_create()
{
    Automaton *autom = SAFEMALLOC(sizeof(Automaton));
    autom->size = 0;
    autom->adj_lists = Array(LinkedList *);
    autom->starting_states = Array(State *);
    autom->states = Array(State *);
    autom->is_determined = 0;
    return autom;
}

void automaton_free(Automaton *automaton)
{
    arr_foreach(State *, s, automaton->states)
    {
        // Frees the states.
        free(s);
    }
    arr_foreach(LinkedList *, list, automaton->adj_lists)
    {
        // Frees the transition list
        list_free(list);
    }
    array_free(automaton->states);
    array_free(automaton->adj_lists);
    array_free(automaton->starting_states);
    free(automaton);
}

State *state_create(int is_terminal)
{
    State *s = SAFEMALLOC(sizeof(State));
    s->terminal = is_terminal;
    return s;
}

void automaton_add_state(Automaton *automaton, State *state, int is_entry)
{
    LinkedList *new_state_list = LinkedList(Transition);
    array_append(automaton->adj_lists, &new_state_list);
    array_append(automaton->states, &state);
    state->id = automaton->size;
    automaton->size++;
    if (is_entry == 1)
    {
        array_append(automaton->starting_states, &state);
    }
}

void automaton_add_transition(Automaton *automaton, State *src, State *dst,
                              Letter value, int epsilon)
{
    LinkedList **adj_list = array_get(automaton->adj_lists, src->id);
    Transition tr;
    tr.target = dst;
    tr.value = value;
    tr.is_epsilon = epsilon;
    if (list_push_back(*adj_list, &tr) == 0)
    {
        errx(1, "Unable to append to the list at address %p",
             (void *)adj_list); // LCOV_EXCL_LINE
    }
}

int automaton_remove_transition(Automaton *automaton, State *src, State *dst,
                                Letter value, int epsilon)
{
    LinkedList *list = *(LinkedList **)array_get(automaton->adj_lists, src->id);
    LinkedList *cpy = list;
    Transition *tr;
    size_t i = 0;
    while (list->next != NULL)
    {
        tr = list->next->data;
        if (tr->target == dst
            && ((epsilon == 1 && tr->is_epsilon == 1) || tr->value == value))
        {
            list_free(list_pop_at(cpy, i));
            return 0;
        }
        i++;
        list = list->next;
    }
    return 1;
}

void automaton_remove_state(Automaton *automaton, State *state)
{
    Transition *tr;
    size_t antoine;
    LinkedList *cpy;
    State *another_state;
    // Remove from adj_lists
    list_free(*(LinkedList **)array_get(automaton->adj_lists, state->id));
    array_remove(automaton->adj_lists, state->id);
    arr_foreach(LinkedList *, list, automaton->adj_lists)
    {
        antoine = 0;
        cpy = list;
        while (list->next != NULL)
        {
            tr = list->next->data;
            list = list->next;
            if (tr->target == state)
            {
                list_free(list_pop_at(cpy, antoine));
            }
            else
            {
                antoine++;
            }
        }
    }
    // Remove from starting_states
    antoine = 0;
    arr_foreach(State *, s, automaton->starting_states)
    {
        if (s == state)
        {
            array_remove(automaton->starting_states, antoine);
            break;
        }
        antoine++;
    }
    // Remove from states array
    for (size_t k = state->id + 1; k < automaton->states->size; k++)
    {
        another_state = *(State **)array_get(automaton->states, k);
        another_state->id -= 1;
    }
    array_remove(automaton->states, state->id);
    automaton->size -= 1;
    free(state);
}

static int is_blank(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/**
 * Go forward in a string until the next non-blank character.
 * @return 0 if end-of-file is reached.
 */
static int move_to_next(const char **string)
{
    while (**string && is_blank(**string))
        (*string)++;
    return **string != 0;
}

/**
 * Read the next adjacent non-blank characaters in the provided string.
 * @return A string containing the symbol. Must be freed by the user.
 */
static char *get_symbol(const char **string)
{
    Array *symbol_array = Array(char);
    while (**string && !is_blank(**string))
        array_append(symbol_array, (*string)++);

    // Turn the array into a string
    char *symbol = calloc(symbol_array->size + 1, sizeof(char));
    size_t i = 0;
    arr_foreach(char, c, symbol_array) symbol[i++] = c;

    array_free(symbol_array);
    return symbol;
}

static int map_state(Array *mapping, size_t alias, size_t real)
{
    if (alias < mapping->size)
    {
        if (*(long *)array_get(mapping, alias) == -1)
        {
            array_set(mapping, alias, &real);
            return 1;
        }
        return 0; // Return 0 if the state is already in the array
    }

    ssize_t x = -1;
    while (mapping->size < alias)
        array_append(mapping, &x);
    array_append(mapping, &real);

    // Return 1 to indicate that the element has been added successfully
    return 1;
}

/**
 * Read a lign from a .daut file and create the new transition in the array.
 * @param automaton The target automaton.
 * @param line The line from the .daut file.
 * @param mapping An array allowing mapping from states in the .daut file
 * to the actual state numbers in the automaton
 */
static void parse_line(Automaton *automaton, const char *line, Array *mapping)
{
    // Get the source state
    if (!move_to_next(&line))
        return; // Ignore if empty

    char *source_symbol = get_symbol(&line);
    size_t source = atol(source_symbol);
    int is_entry = strcmp(source_symbol, "$") == 0;
    if (!is_entry && source == 0 && strcmp(source_symbol, "0") != 0)
        errx(EXIT_FAILURE, // LCOV_EXCL_LINE
             "Invalid state: %s: only integers are supported for now",
             source_symbol); // LCOV_EXCL_LINE
    free(source_symbol);

    // Get the middle arrow
    if (!move_to_next(&line))
        errx(EXIT_FAILURE, "Expected '->' after state"); // LCOV_EXCL_LINE
    char *arrow_symbol = get_symbol(&line);
    if (strcmp(arrow_symbol, "->") != 0)
        errx(EXIT_FAILURE, "Expected '->' after state"); // LCOV_EXCL_LINE
    free(arrow_symbol);

    // Get the target state
    if (!move_to_next(&line))
        errx(EXIT_FAILURE, "Expected a target state"); // LCOV_EXCL_LINE
    char *target_symbol = get_symbol(&line);
    size_t target = atol(target_symbol);
    int is_terminal = strcmp(target_symbol, "$") == 0;
    if (!is_terminal && target == 0 && strcmp(target_symbol, "0") != 0)
        errx(EXIT_FAILURE, // LCOV_EXCL_LINE
             "Invalid state: %s: only integers are supported for now",
             target_symbol); // LCOV_EXCL_LINE
    free(target_symbol);

    // Get the value of the transition
    int is_epsilon = !move_to_next(&line);
    Letter value = 0;
    if (!is_epsilon)
        value = *(line++);

    // Get the states, add them if it they don't exist yet
    State *src_state;
    if (!is_entry && map_state(mapping, source, automaton->size))
    {
        src_state = State(is_terminal);
        automaton_add_state(automaton, src_state, 0);
    }
    else if (!is_entry)
    {
        size_t *index = array_get(mapping, source);
        src_state = *(State **)array_get(automaton->states, *index);
        src_state->terminal = src_state->terminal || is_terminal;
    }
    State *dst_state;
    if (!is_terminal && map_state(mapping, target, automaton->size))
    {
        dst_state = State(0);
        automaton_add_state(automaton, dst_state, is_entry);
    }
    else if (!is_terminal)
    {
        size_t *index = array_get(mapping, target);
        dst_state = *(State **)array_get(automaton->states, *index);
        if (is_entry)
        {
            int is_dst_entry = 0;
            arr_foreach(State *, curr,
                        automaton->starting_states) if (curr->id == *index)
            {
                is_dst_entry = 1;
                break;
            }
            if (!is_dst_entry)
                array_append(automaton->starting_states, &dst_state);
        }
    }

    if (!is_terminal && !is_entry)
        automaton_add_transition(automaton, src_state, dst_state, value,
                                 is_epsilon);
}


Automaton *automaton_from_daut(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        err(EXIT_FAILURE, "Couldn't open %s", filename); // LCOV_EXCL_LINE
    Automaton *automaton = Automaton();

    char *line = NULL;
    size_t linecap = 0;
    Array *mapping = Array(size_t);
    while (getline(&line, &linecap, file) > 0)
        parse_line(automaton, line, mapping);
    free(line);
    fclose(file);
    array_free(mapping);
    return automaton;
}

void print_automaton(Automaton *aut)
{
    printf("Automaton: \n\nStates:\n\n");
    arr_foreach(State *, state, aut->states)
    {
        printf("%zu : terminal: %s\n", state->id,
               state->terminal ? "true" : "false");
    }
    printf("\nStarting states:\n\n");
    arr_foreach(State *, starting_state, aut->starting_states)
    {
        printf("%zu\n", starting_state->id);
    }
    printf("\nTransitions :\n\n");
    size_t index = 0;
    arr_foreach(LinkedList *, transitions, aut->adj_lists)
    {
        transitions = transitions->next;
        while (transitions != NULL)
        {
            Transition *transition = transitions->data;
            char transition_str[5] = { 0 };
            if (transition->is_epsilon)
                memcpy(transition_str, "ε", 3);
            else
                transition_str[0] = transition->value;
            printf("%zu - %s -> %zu\n", index, transition_str,
                   transition->target->id);
            transitions = transitions->next;
        }

        index += 1;
    }
    printf("\n");
}

void automaton_to_dot(Automaton *aut)
{
    size_t index = 0;
    printf("digraph{\n  rankdir=LR;\n");
    arr_foreach(State *, start, aut->starting_states)
    {
        printf("  node [shape = point ]; q%zu\n", start->id);
    }

    printf("  node [shape = doublecircle]; \n");
    arr_foreach(State *, state, aut->states)
    {
        if (state->terminal)
            printf("  %zu;\n", state->id);
    }
    printf("  node [shape = circle];\n");
    arr_foreach(State *, start_2, aut->starting_states)
    {
        printf("  q%zu -> %zu\n", start->id, start->id);
    }
    arr_foreach(LinkedList *, transitions, aut->adj_lists)
    {
        transitions = transitions->next;
        while (transitions != NULL)
        {
            Transition *transition = transitions->data;
            char transition_str[5] = { 0 };
            if (transition->is_epsilon)
                memcpy(transition_str, "ε", 3);
            else
                transition_str[0] = transition->value;
            printf("  %zu -> %zu[label=\"%s\"]\n", index,
                   transition->target->id, transition_str);
            transitions = transitions->next;
        }

        index += 1;
    }
    printf("}\n");
}
