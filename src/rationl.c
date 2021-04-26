#include <string.h>
#include <stdlib.h>
#include "utils/memory_utils.h"
#include "datatypes/bin_tree.h"
#include "datatypes/array.h"
#include "automaton/automaton.h"
#include "matching/matching.h"
#include "automaton/thompson.h"
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
    reg_t re;
    re.aut = aut;
    re.pattern = malloc((strlen(pattern) + 1) *sizeof(char));
    strcpy(re.pattern, pattern);
    bintree_free(tree);
    array_free(arr);
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
    Array *arr = search_nfa(re.aut, str);
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
