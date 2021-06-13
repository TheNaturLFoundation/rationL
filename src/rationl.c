#include <string.h>
#include <stdlib.h>
#include "utils/memory_utils.h"
#include "datatypes/bin_tree.h"
#include "datatypes/array.h"
#include "automaton/automaton.h"
#include "matching/matching.h"
#include "automaton/thompson.h"
#include "automaton/delete_eps.h"
#include "automaton/prune.h"
#include "automaton/minimization.h"
#include "automaton/stringify.h"
#include "parsing/lexer.h"
#include "parsing/parsing.h"

typedef struct reg_t
{
    Automaton* aut;
    char* pattern;
} reg_t;

typedef struct match
{
    const char *string;
    size_t start;
    size_t length;
    size_t nb_groups;
    char **groups;
} match;

reg_t regex_compile(char* pattern)
{
    Array *arr = tokenize(pattern);
    BinTree *tree = parse_symbols(arr);
    Automaton *aut = thompson(tree);
    automaton_delete_epsilon_tr(aut);
    automaton_prune(aut);
    Automaton *minimized = minimize(aut);
    automaton_free(aut);

    reg_t re;
    re.aut = minimized;
    re.pattern = malloc((strlen(pattern) + 1) * sizeof(char));
    strcpy(re.pattern, pattern);
    bintree_free(tree);
    free_tokens(arr);
    return re;
}

reg_t regex_read_daut(char *path)
{
    Automaton *aut = automaton_from_daut(path, 255);
    automaton_delete_epsilon_tr(aut);
    automaton_prune(aut);
    Automaton *minimized = minimize(aut);
    automaton_free(aut);

    reg_t re;
    re.aut = minimized;
    re.pattern = stringify(minimized);

    return re;
}

void regex_free(reg_t re)
{
    automaton_free(re.aut);
    free(re.pattern);
}

void match_free(match *match)
{
    free_match((Match *) match);
}

match *regex_match(reg_t re, char* str)
{
    return (match *)match_nfa(re.aut, str);
}

size_t regex_search(reg_t re, char *str, match **groups[])
{
    Array *arr;
    if (re.aut->is_determined)
        arr = search_dfa(re.aut, str);
    else
        arr = search_nfa(re.aut, str);

    size_t n = arr->size;
    *groups = SAFEMALLOC(n * sizeof(char *));

    for (size_t i = 0; i < n; i++)
        (*groups)[i] = *(match **)array_get(arr, i);

    array_free(arr);
    return n;
}

char *regex_sub(reg_t re, char *str, char *sub)
{
    return replace_nfa(re.aut, str, sub);
}
