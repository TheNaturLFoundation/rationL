#include "automaton.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "datatypes/array.h"
#include "utils/memory_utils.h"

LinkedList *get_matrix_elt(const Automaton *automaton, size_t state_id,
                           Letter value, int is_epsilon)
{
    size_t lookup_index = (is_epsilon != 0) ? EPSILON_INDEX : value;
    int index = automaton->lookup_table[lookup_index];
    if (index == -1)
    {
        return NULL;
    }

    return matrix_get(automaton->transition_table, index, state_id);
}

int state_is_entry(Automaton * automaton, State * s)
{
    arr_foreach(State *, entry, automaton->starting_states)
    {
        if(entry == s)
            return 1;
    }

    return 0;
}

Automaton *automaton_create(size_t state_count, size_t letter_count)
{
    Automaton *autom = SAFEMALLOC(sizeof(Automaton));
    autom->size = 0;
    autom->capacity = state_count;
    autom->transition_table = Matrix(state_count, letter_count);
    autom->starting_states = Array(State *);
    autom->states = Array(State *);
    autom->lookup_table = SAFEMALLOC(sizeof(int) * 257);
    for (int i = 0; i < 257; i++)
        autom->lookup_table[i] = -1;
    autom->lookup_used = 0;
    autom->is_determined = 0;
    return autom;
}

void automaton_free(Automaton *automaton)
{
    // Frees the states.
    arr_foreach(State *, s, automaton->states) free(s);
    array_free(automaton->states);
    matrix_free(automaton->transition_table);
    array_free(automaton->starting_states);
    free(automaton->lookup_table);
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
    Matrix *mat = automaton->transition_table;
    
    if(automaton->size + 1 > automaton->capacity)
    {
        automaton->capacity += 1;
        matrix_add_row(automaton->transition_table);
    }
    
    array_append(automaton->states, &state);
    state->id = automaton->size;
    automaton->size++;
    if (is_entry == 1)
        array_append(automaton->starting_states, &state);
}

void automaton_add_transition(Automaton *automaton, State *src, State *dst,
                              Letter value, int epsilon)
{
    size_t i = (epsilon != 0) ? EPSILON_INDEX : value;
    int mat_col = automaton->lookup_table[i];
    
    if (mat_col == -1)
    {
        automaton->lookup_table[i] = automaton->lookup_used;
        mat_col = automaton->lookup_used;
        automaton->lookup_used++;
    }

    LinkedList *trans =
        matrix_get(automaton->transition_table, mat_col, src->id);

    if (trans == NULL)
    {
        trans = LinkedList(State *);
        matrix_set(automaton->transition_table, mat_col, src->id, trans);
    }
    if (!list_push_back(trans, &dst))
        errx(
            EXIT_FAILURE, // LCOV_EXCL_LINE
            "Unable to append to the list at address %p letter = %c", // LCOV_EXCL_LINE
            (void *)trans, value); // LCOV_EXCL_LINE
}

int automaton_remove_transition(Automaton *automaton, State *src, State *dst,
                                Letter value, int epsilon)
{
    LinkedList *start = get_matrix_elt(automaton, src->id, value, epsilon);
    // Skip the sentinel
    if(start != NULL)
    {
        for (LinkedList *trans = start->next; trans != NULL; trans = trans->next)
        {
            State *curr_dst = *(State **)trans->data;
            if (curr_dst->id == dst->id)
            {
                trans->previous->next = trans->next;
                if(trans->next != NULL)
                {
                    trans->next->previous = trans->previous;
                }
                trans->next = NULL;
                list_free(trans);
                if(start->next == NULL)
                {
                    size_t i = (epsilon != 0) ? EPSILON_INDEX : value;
                    list_free(start);
                    matrix_set(automaton->transition_table, 
                        automaton->lookup_table[i], src->id, NULL);
                }
                return 0;
            }
        } // LCOV_EXCL_LINE
    }
    return 1;
}

void automaton_remove_state(Automaton *automaton, State *state)
{
    // Remove all transitions pointing to it
    State * src;
    for(size_t symb = 0; symb < NUMBER_OF_SYMB; symb ++)
    {
        for(size_t i = 0; i < state->id; i++)
        {
            src = *(State **)array_get(automaton->states, i);
            automaton_remove_transition(automaton, src, state, symb, symb == EPSILON_INDEX);
        }

        for(size_t i = state->id + 1; i < automaton->size; i++)
        {
            src = *(State **)array_get(automaton->states, i);
            automaton_remove_transition(automaton, src, state, symb, symb == EPSILON_INDEX);
        }
    }

    // Remove from starting_states
    size_t antoine = 0;
    arr_foreach(State *, s, automaton->starting_states)
    {
        if (s->id == state->id)
        {
            array_remove(automaton->starting_states, antoine);
            break;
        }
        antoine++;
    }

    // Remove from states array
    for (size_t k = state->id + 1; k < automaton->states->size; k++)
    {
        State *another_state = *(State **)array_get(automaton->states, k);
        another_state->id -= 1;
    }

    if(automaton->transition_table != NULL)
    {
        //Deal with matrix
            //We start by freeing the lists of the line we want to delete
        for (size_t x = 0; x < automaton->transition_table->width; x++)
            list_free(matrix_get(automaton->transition_table, x, state->id));
            
            //Then, we shift the matrix below the deleted line by one line
        for (size_t k = state->id; k < automaton->states->size - 1; k++)
        {
            for (size_t x = 0; x < automaton->transition_table->width; x++)
            {
                LinkedList *list =
                    matrix_get(automaton->transition_table, x, k + 1);
                matrix_set(automaton->transition_table, x, k, list);
            }
        }

        for (size_t x = 0; x < automaton->transition_table->width; x++)
        {
            LinkedList *null = NULL;
            matrix_set(automaton->transition_table, x, automaton->size - 1, null);
        }

            //Then, we change the matrix height and width
        automaton->transition_table->height --;
        /*
            //Finally we perform the realloc
        automaton->transition_table = SAFEREALLOC(automaton->transition_table,
            automaton->transition_table->width * automaton->transition_table->height * sizeof(LinkedList *));
        
        I don't know if reallocing is the right thing to do as we malloc what is supposed
        to be a fixed sized automaton, however the code of add_automaton_allows to go over that
        number
        */
    }

    array_remove(automaton->states, state->id);
    automaton->size -= 1;

    free(state);
}

Automaton *automaton_copy(Automaton *source)
{
    Automaton *copy = Automaton(source->size, source->transition_table->width);

    {
        arr_foreach(State *, state, source->states)
        {
            State *new_state = State(state->terminal);
            int is_entry = 0;
            arr_foreach(State *, starting_state, source->starting_states)
                if (starting_state->id == state->id)
                {
                    is_entry = 1;
                    break;
                }
            automaton_add_state(copy, new_state, is_entry);
        }
    }

    for (size_t i = 0; i < NUMBER_OF_SYMB; i++)
    {
        for (size_t j = 0; j < source->size; j++)
        {
            State *src = *(State **)array_get(copy->states, j);
            LinkedList *transitions = get_matrix_elt(source, j, i, i == EPSILON_INDEX);
            list_foreach(State *, old_dest, transitions)
                {
                    State *dest = *(State **)array_get(copy->states, old_dest->id);
                    automaton_add_transition(copy, src, dest, i,
                                             i == EPSILON_INDEX);
                }
        }
    }

    return copy;
}

// Helpers for daut conversion

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
    char *symbol = SAFECALLOC(symbol_array->size + 1, sizeof(char));
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
static void parse_daut_line(Automaton *automaton, const char *line,
                            Array *mapping)
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

Automaton *automaton_from_daut(const char *filename, size_t size)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        err(EXIT_FAILURE, "Couldn't open %s", filename); // LCOV_EXCL_LINE
    Automaton *automaton = Automaton(size, NUMBER_OF_SYMB);

    char *line = NULL;
    size_t linecap = 0;
    Array *mapping = Array(size_t);
    while (getline(&line, &linecap, file) > 0)
        parse_daut_line(automaton, line, mapping);
    free(line);
    fclose(file);
    array_free(mapping);
    return automaton;
}

// LCOV_EXCL_START
void automaton_to_dot(Automaton *aut)
{
    puts("digraph{\n  rankdir=LR;");
    arr_foreach(State *, start, aut->starting_states)
        printf("  node [shape = point ]; q%zu\n", start->id);

    puts("  node [shape = doublecircle];");
    arr_foreach(State *, state, aut->states) if (state->terminal)
        printf("  %zu;\n", state->id);
    puts("  node [shape = circle];");
    arr_foreach(State *, state_2, aut->starting_states)
        printf("  q%zu -> %zu\n", state_2->id, state_2->id);

    for (size_t src = 0; src < aut->size; src++)
    {
        const Letter eps_index = 0;
        for (Letter c = 0; c < 255; c++)
        {
            int is_epsilon = c == eps_index;
            LinkedList *list = get_matrix_elt(aut, src, c, is_epsilon);
            Letter transition_str[3] = { 0 };
            if (is_epsilon)
                memcpy(transition_str, "ε", 3);
            else
                transition_str[0] = c;
            list_foreach(State *, target, list)
                printf("  %zu -> %zu[label=\"%s\"]\n", src, target->id,
                       transition_str);
        }
    }
    puts("}");
}
// LCOV_EXCL_STOP
