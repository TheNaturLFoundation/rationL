#include <string.h>
#include "datatypes/bin_tree.h"
#include <stdlib.h>
#include "datatypes/array.h"
#include "automaton/automaton.h"
#include "matching/matching.h"
#include "automaton/thompson.h"
#include "parsing/lexer.h"
#include "parsing/parsing.h"

typedef struct regex_t
{
    Automaton* aut;
    char* pattern;
} regex_t;

regex_t regex_compile(char* pattern)
{
    Array *arr = tokenize(pattern);
    BinTree *tree = parse_symbols(arr);
    Automaton *aut = thompson(tree);
    regex_t re;
    re.aut = aut;
    re.pattern = malloc((strlen(pattern) + 1) *sizeof(char));
    strcpy(re.pattern, pattern);
    bintree_free(tree);
    array_free(arr);
    return re;
}

void regex_free(regex_t re)
{
    automaton_free(re.aut);
    free(re.pattern);
}

int regex_match(regex_t re, char* str)
{
    return match_nfa(re.aut, str);
}
