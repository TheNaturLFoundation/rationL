#include "automaton.h"

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "datatypes/array.h"
#include "utils/memory_utils.h"
#include "utils/errors.h"

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

    autom->nb_groups = 0;
    autom->entering_transitions = Map(Transition, Map *, hash_transition, compare_transitions);
    autom->leaving_transitions = Map(Transition, Map *, hash_transition, compare_transitions);
    
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
    
    Map * cpy = automaton->entering_transitions;
    //Need a foreach value in Map
    //Sorry...
    {
        map_foreach_value(
            Map *, set, cpy,
            {
                map_free(set);
            })
        map_free(automaton->entering_transitions);
    }

    cpy = automaton->leaving_transitions;
    //Need a foreach value in Map
    map_foreach_value(
        Map *, set_, cpy,
        {
            map_free(set_);
        })

    map_free(automaton->leaving_transitions);
    
    free(automaton);
}

Transition _generate_transition(State * src, State * dst, Letter value,int epsilon)
{
    size_t src_id = (src == NULL) ? 0 : src->id + 1;
    size_t dst_id = (dst == NULL) ? 0 : dst->id + 1;
    if((dst == NULL) || (src == NULL))
    {
        epsilon = 1;
    }
    if(epsilon != 0)
    {
        value = 0;
    }

    Transition tr = {src_id, dst_id, value, epsilon};

    return tr;
}

void _remove_transition_from_map(Map * map, Transition * tr)
{
    Map ** group_set_ptr = map_delete(map, tr);
    if(group_set_ptr != NULL)
    {
        map_free(*group_set_ptr);
    }
    free(group_set_ptr);

    free(map_delete(map, tr));
}

void _automaton_remove_transition_from_maps(Automaton * automaton, 
    State * src, State * dst, Letter value, int epsilon)
{
    Transition tr = _generate_transition(src, dst, 
        value, epsilon);
    _remove_transition_from_map(automaton->entering_transitions, &tr);
    _remove_transition_from_map(automaton->leaving_transitions, &tr);
}


State *state_create(int is_terminal)
{
    State *s = SAFEMALLOC(sizeof(State));
    s->terminal = is_terminal;
    return s;
}

void automaton_add_state(Automaton *automaton, State *state, int is_entry)
{

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
    _automaton_remove_transition_from_maps(automaton, src, dst, value, epsilon);
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
                list_free_from(trans);
                if(start->next == NULL)
                {
                    size_t i = (epsilon != 0) ? EPSILON_INDEX : value;
                    list_free_from(start);
                    matrix_set(automaton->transition_table, 
                        automaton->lookup_table[i], src->id, NULL);
                }
                return 0;
            }
        } // LCOV_EXCL_LINE
    }
    return 1;
}

void _free_start_marked_as_entering(Automaton * automaton, State * s)
{
    Transition tr = _generate_transition(NULL, s, 0, 1);
    Map ** set_ptr = map_delete(automaton->entering_transitions, &tr);
    if(set_ptr != NULL)
    {
        map_free(*set_ptr);
    }
    free(set_ptr);
}


void automaton_remove_state(Automaton *automaton, State *state)
{
    // Remove all transitions pointing to it
    State * src;
    LinkedList * tr_list;
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
            _free_start_marked_as_entering(automaton, s);
            break;
        }
        antoine++;
    }

    if(automaton->transition_table != NULL)
    {
        //Deal with matrix
        //We start by freeing the lists of the line we want to delete
        for (size_t symb = 0; symb < NUMBER_OF_SYMB; symb++)
        {
            if(automaton->lookup_table[symb] != -1)
            {
                tr_list = matrix_get(automaton->transition_table, automaton->lookup_table[symb], state->id);
                list_foreach(State *, trg, tr_list)
                {
                    _automaton_remove_transition_from_maps(automaton, state, trg, symb, symb == EPSILON_INDEX);
                }
                list_free(tr_list);
            }
        }
        
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

    // Remove from states array
    for (size_t k = state->id + 1; k < automaton->states->size; k++)
    {
        State *another_state = *(State **)array_get(automaton->states, k);
        another_state->id -= 1;
    }

    array_remove(automaton->states, state->id);
    automaton->size -= 1;

    free(state);
}

Set * set_cpy(Set * set)
{
    Set * ret = Set(size_t, hash_size_t, compare_size_t);
    map_foreach_key(
        size_t, grp, set,
        {
            set_add(set, &grp);
        }
    )
    return ret;
}

Map * _map_cpy(Map * src)
{
    Map * ret =  Map(Transition, Map *, hash_transition, compare_transitions);
    Set * set;
    map_foreach_key(
        Transition, tr, src,
        {
            set = map_get(src, &tr);
            Set *copy = set_cpy(set);
            map_set(ret, &tr, &copy);
        }
    )
    return ret;
}

Automaton *automaton_copy(Automaton *source)
{
    Automaton *copy = Automaton(source->size, source->transition_table->width);
    copy->is_determined = source->is_determined;
    copy->nb_groups = source->nb_groups;

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

    copy->entering_transitions = _map_cpy(source->entering_transitions);
    copy->leaving_transitions =_map_cpy(source->leaving_transitions);
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
 * @return -1 if there is an errno and sets the naturl_errno accordingly
 */
static int parse_daut_line(Automaton *automaton, const char *line,
                            Array *mapping)
{
    // Get the source state
    if (!move_to_next(&line))
        return 0; // Ignore if empty

    char *source_symbol = get_symbol(&line);
    size_t source = atol(source_symbol);
    int is_entry = strcmp(source_symbol, "$") == 0;

    if (!is_entry && source == 0 && strcmp(source_symbol, "0") != 0)
    {
        free(source_symbol);
        rationl_errno = EBADFMT;
        return -1;
    }
    free(source_symbol);

    // Get the middle arrow
    if (!move_to_next(&line))
    {
        rationl_errno = EBADFMT;
        return -1;
    }
    char *arrow_symbol = get_symbol(&line);
    if (strcmp(arrow_symbol, "->") != 0)
    {
        free(arrow_symbol);
        rationl_errno = EBADFMT;
        return -1;
    }
    free(arrow_symbol);

    // Get the target state
    if (!move_to_next(&line))
    {
        rationl_errno = EBADFMT;
        return -1;
    }

    char *target_symbol = get_symbol(&line);
    size_t target = atol(target_symbol);
    int is_terminal = strcmp(target_symbol, "$") == 0;
    if (!is_terminal && target == 0 && strcmp(target_symbol, "0") != 0)
    {
        free(target_symbol);
        rationl_errno = EBADFMT;
        return -1;
    }
    free(target_symbol);

    // Get the value of the transition
    const char *before = line;
    int is_epsilon = !move_to_next(&line);
    Letter value = 0;
    if (((*line == '<' || *line == '>') && line) && line[1])
        line = before;
    else if (!is_epsilon)
        value = *(line++);

    // Test if the transition is marked as entering/leaving
    Array *entering = Array(int);
    Array *leaving = Array(int);
    while (move_to_next(&line))
    {
        switch (*(line++))
        {
        case '>': {
            char *no_symbol = get_symbol(&line);
            int no = atol(no_symbol); // Assume it's right
            if (no + 1 > automaton->nb_groups)
                automaton->nb_groups = no + 1;
            free(no_symbol);
            array_append(entering, &no);
            break;
        }
        case '<': {
            char *no_symbol = get_symbol(&line);
            int no = atol(no_symbol);
            if (no + 1 > automaton->nb_groups)
                automaton->nb_groups = no + 1;
            free(no_symbol);
            array_append(leaving, &no);
            break;
        }
        default:
            printf("Got '%c'\n", *(line - 1));
            rationl_errno = EBADFMT;
            return -1;
        }
    }

    // Get the states, add them if it they don't exist yet
    State *src_state = NULL;
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
    State *dst_state = NULL;
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

    arr_foreach(int, entering_no, entering)
        automaton_mark_entering(automaton, src_state, dst_state, value,
                                is_epsilon, entering_no);
    arr_foreach(int, leaving_no, leaving)
        automaton_mark_leaving(automaton, src_state, dst_state, value,
                               is_epsilon, leaving_no);

    return 0;
}

Automaton *automaton_from_daut(const char *filename, size_t size)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        rationl_errno = ENOFILE;
        return NULL;
    }
    Automaton *automaton = Automaton(size, NUMBER_OF_SYMB);

    char *line = NULL;
    size_t linecap = 0;
    Array *mapping = Array(size_t);
    while (getline(&line, &linecap, file) > 0)
        if (parse_daut_line(automaton, line, mapping) == -1)
            return NULL;
    free(line);
    fclose(file);
    array_free(mapping);
    return automaton;
}

// LCOV_EXCL_START
void automaton_to_dot(Automaton *aut)
{
    char * entering_str;
    char * leaving_str;
    State * s;
    puts("digraph{\n  rankdir=LR;");
    arr_foreach(State *, start, aut->starting_states)
        printf("  node [shape = point ]; q%zu\n", start->id);

    puts("  node [shape = doublecircle];");
    arr_foreach(State *, state, aut->states)
    { 
        if (state->terminal)
        {
            entering_str = stringify_set(get_entering_groups(aut,
                state, NULL, 0, 1), 'E');
            leaving_str = stringify_set(get_leaving_group(aut,
                state, NULL, 0, 1), 'S');
            printf("  %zu[xlabel=\"%s %s\"];\n", state->id, entering_str,
                leaving_str);
            free(entering_str);
            free(leaving_str);
        }
    }
    puts("  node [shape = circle];");
    arr_foreach(State *, state_2, aut->starting_states)
    {
        entering_str = stringify_set(get_entering_groups(aut,
                    NULL, state_2, 0, 1), 'E');
        printf("  q%zu -> %zu[label=\"%s\"]\n", state_2->id, state_2->id, 
            entering_str);
        free(entering_str);
    }
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
            {
                s = *(State **)array_get(aut->states, src);
                entering_str = stringify_set(get_entering_groups(aut,
                    s, target, c, c == eps_index), 'E');
                leaving_str = stringify_set(get_leaving_group(aut,
                    s, target, c, c == eps_index), 'S');
                printf("  %zu -> %zu[label=\"%s %s %s\"]\n", src, target->id,
                       transition_str, entering_str, leaving_str);
                
                free(entering_str);
                free(leaving_str);
            }
        }
    }
    puts("}");
}
// LCOV_EXCL_STOP

int _check_state(Automaton * automaton, State * s)
{
    int is_in = 0;
    State * maybe_s;
    if(s->id < automaton->size)
    {
        maybe_s = *(State **)array_get(automaton->states, s->id);
        if(maybe_s == s)
            is_in = 1;
    }
    return is_in;
}

int automaton_is_transition(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon)
{
    if(src == NULL)
        return _check_state(automaton, dst);
    if(dst == NULL)
        return _check_state(automaton, src);

    size_t letter = (epsilon != 0) ? EPSILON_INDEX : value;
    int index = automaton->lookup_table[letter];
    if(index == -1)
        return 0;
    
    if(_check_state(automaton, src) != 1 || _check_state(automaton, dst) != 1)
        return 0;

    LinkedList * list = matrix_get(automaton->transition_table, index, src->id);
    list_foreach(State *, pdst, list)
    {
        if(pdst == dst)
            return 1;
    }
    return 0;
}

void _check_transiton_in_automaton(Automaton * automaton, State * src, State * dst, 
    Letter value, int epsilon)
{
    if(automaton_is_transition(automaton, src, dst, value, epsilon) != 1)
        errx(1, "You know what ? shit happens");
}

void _mark_tr_to_map(Map * map, Transition * tr, size_t group)
{
    Map ** set_ptr = map_get(map, tr);
    Map * set;
    if(set_ptr == NULL)
    {
        set = Set(size_t, hash_size_t, compare_size_t);
        map_set(map, tr, &set);
    }
    else
    {
        set = *set_ptr;
    }
    set_add(set, &group);
}

void _mark_to_map(Map * map, State * src, State * dst, Letter value, 
    int epsilon, size_t group)
{
    Transition tr = _generate_transition(src, dst, value, epsilon);
    _mark_tr_to_map(map, &tr, group);
}

void automaton_mark_entering(Automaton * automaton, State * src, State * dst, 
    Letter value, int epsilon, size_t group)
{
    if (group + 1 > automaton->nb_groups)
        automaton->nb_groups = group + 1;
    _check_transiton_in_automaton(automaton, src, dst, value, epsilon);
    _mark_to_map(automaton->entering_transitions, src, dst, value, epsilon, group);
}

void automaton_mark_leaving(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon, size_t group)
{
    if (group + 1 > automaton->nb_groups)
        automaton->nb_groups = group + 1;
    _check_transiton_in_automaton(automaton, src, dst, value, epsilon);
    _mark_to_map(automaton->leaving_transitions, src, dst, value, epsilon, group);
}

Map * _get_set_from_tr(Map * map, State * src, State * dst, 
    Letter value, int epsilon)
{
    Transition tr = _generate_transition(src, dst, value, epsilon);
    Map ** ptr = map_get(map, &tr);
    
    if(ptr == NULL)
        return NULL;
    
    return *ptr;
}

Map * get_entering_groups(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon)
{
    return _get_set_from_tr(automaton->entering_transitions, src, dst, value, epsilon);
}

Map * get_leaving_group(Automaton * automaton, State * src, State * dst,
    Letter value, int epsilon)
{
    return _get_set_from_tr(automaton->leaving_transitions, src, dst, value, epsilon);
}

void automaton_clear_state_terminal(Automaton * automaton, State * state)
{
    if(state->terminal == 0)
        return;
    _automaton_remove_transition_from_maps(automaton, state, NULL, 0, 1);
    state->terminal = 0;
}

void automaton_clear_state_entry(Automaton * automaton, size_t index)
{
    State * s = *(State **)array_get(automaton->starting_states, index);
    _automaton_remove_transition_from_maps(automaton, NULL, s, 0, 1);
    array_remove(automaton->starting_states, index);
}
