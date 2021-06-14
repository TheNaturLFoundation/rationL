#include "stringify.h"
#include <string.h>
#include "automaton/minimization.h"
#include "utils/memory_utils.h"

static Array **build_adj_mat(const Automaton *automaton);
static void free_adj_mat(Array **mat, size_t size);
static void short_circuit(size_t state, size_t prev, Array **mat, size_t size);

static const char open_par = '(';
static const char close_par = ')';
static const char star = '*';
static const char pipe = '|';
static const char column = ':';
static const char question = '?';


/**
 * Uses the state elimination Brozowski & McCluskey algorithm.
 */
char *stringify(Automaton *automaton)
{
    automaton = minimize(automaton);
    Array **mat = build_adj_mat(automaton);
    size_t size = automaton->size + 2;

    for (size_t curr = 0; curr < automaton->size + 1; curr++)
    {
        short_circuit(curr, 0, mat, size);
        for (size_t prev = curr + 1; prev < size; prev++)
            short_circuit(curr, prev, mat, size);
    }

    Array *final = mat[size - 1];
    char *string = NULL;
    if (final != NULL)
    {
        string = SAFECALLOC(final->size + 1, sizeof(char));
        memcpy(string, final->data, final->size);
    }

    free_adj_mat(mat, size);

    return string;
}

static void short_circuit(size_t state, size_t prev, Array **mat, size_t size)
{
    size_t prev_index = prev * size + state;
    if (mat[prev_index] == NULL)
        return;

    Array *self_trans = Array(char);
    if (mat[state * size + state] != NULL)
    {
        int par = mat[state * size + state]->size > 1;
        if (par)
        {
            array_append(self_trans, &open_par);
            array_append(self_trans, &question);
            array_append(self_trans, &column);
        }
        arr_foreach(char, c, mat[state * size + state])
            array_append(self_trans, &c);
        if (par)
            array_append(self_trans, &close_par);
        array_append(self_trans, &star);
    }

    for (size_t next = state + 1; next < size; next++)
    {
        size_t next_index = state * size + next;
        if (mat[next_index] == NULL)
            continue;

        // Bluid the transition from prev to next
        // short-circuiting the current state.
        Array *trans = Array(char);
        arr_foreach(char, prev_c, mat[prev_index])
            array_append(trans, &prev_c);
        arr_foreach(char, self_c, self_trans)
            array_append(trans, &self_c);
        arr_foreach(char, next_c, mat[next_index])
            array_append(trans, &next_c);

        // If there is already a transition, "or" it, else add it
        Array *curr_trans = mat[prev * size + next];
        if (curr_trans == NULL)
            mat[prev * size + next] = trans;
        else
        {
            array_insert(curr_trans, 0, &column);
            array_insert(curr_trans, 0, &question);
            array_insert(curr_trans, 0, &open_par);

            array_append(curr_trans, &pipe);
            arr_foreach(char, c, trans)
                array_append(curr_trans, &c);
            array_append(curr_trans, &close_par);

            array_free(trans);
        }
    }

    array_free(self_trans);
}

/**
 * Builds an adjacency matrix associated to an automaton.
 * Each transition is an array representing a string.
 */
static Array **build_adj_mat(const Automaton *automaton)
{
    // Attach a starting end ending state to the automaton
    size_t size = automaton->size + 2;
    Array **mat = SAFECALLOC(size * size, sizeof(Array *));

    // Attach starting states
    arr_foreach(State *, starting_state, automaton->starting_states)
        mat[starting_state->id + 1] = Array(char);

    for (size_t i = 0; i < automaton->size; i++)
        for (size_t letter = 0; letter < 256; letter++)
        {
            // DFA -> no epsilon-transition
            LinkedList *list = get_matrix_elt(automaton, i, letter, 0);
            if (list_empty(list))
                continue;

            size_t src = i + 1;
            State *dst_state = *(State **)list->next->data;
            size_t dst = dst_state->id + 1;

            Array *trans = mat[src * size + dst];
            if (trans == NULL)
                trans = mat[src * size + dst] = Array(char);
            else
            {
                // Multiple transition are directly turned to OR
                if (trans->size == 1)
                {
                    array_insert(trans, 0, &column);
                    array_insert(trans, 0, &question);
                    array_insert(trans, 0, &open_par);

                    array_append(trans, &close_par);
                }
                array_insert(trans, trans->size - 1, &pipe);
            }
            array_insert(trans, trans->size > 1 ? trans->size - 1 : 0, &letter);

            // Attach end states
            if (dst_state->terminal)
                mat[dst * size + (size - 1)] = Array(char);
        }

    return mat;
}

static void free_adj_mat(Array **mat, size_t size)
{
    for (size_t i = 0; i < size * size; i++)
        if (mat[i] != NULL)
            array_free(mat[i]);

    free(mat);
}
