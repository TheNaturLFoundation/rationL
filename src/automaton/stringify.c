#include "stringify.h"
#include "utils/memory_utils.h"

static Array **build_adj_mat(const Automaton *automaton);
static void free_adj_mat(const Array **mat, size_t size);

/**
 * Uses the state elimination Brozowski & McCluskey algorithm.
 */
char *stringify(const Automaton *automaton)
{
    Array **mat = build_adj_mat(automaton);

    for (size_t curr = 0; curr < automaton->size; curr++)
    {
         for (size_t prev = curr; prev < automaton->size; prev++)
             for (size_t next = curr; next < automaton->size; next++)
             {

             }
    }

    free_adj_mat(mat, automaton->size);

    return NULL;
}

/**
 * Builds an adjacency matrix associated to an automaton.
 * Each transition is an array representing a string.
 */
static Array **build_adj_mat(const Automaton *automaton)
{
    Array **mat =
        SAFECALLOC(automaton->size * automaton->size, sizeof(Array *));

    for (size_t src = 0; src < automaton->size; src++)
        for (size_t letter = 0; letter < 256; letter++)
        {
            // DFA -> no epsilon-transition
            LinkedList *list = get_matrix_elt(automaton, src, letter, 0);
            if (list_empty(list))
                continue;

            size_t dst = (*(State **)list->next->data)->id;
            mat[src * automaton->size + dst] = Array(char);
            array_append(mat[src * automaton->size + dst], &letter);
        }

    return mat;
}

static void free_adj_mat(const Array **mat, size_t size)
{
    for (size_t i = 0; i < size * size; i++)
        array_free(mat[i]);

    free(mat);
}
