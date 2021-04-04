#include <string.h>
#include "rationl/matching.h"
#include "rationl/thompson.h"
#include "rationl/automaton.h"
#include "rationl/parsing.h"
#include "rationl.h"

regex_t compile(char* pattern)
{
    Array *arr = tokenize(pattern);
    BinTree *tree = parse_symbols(arr);
    Automaton *aut = thompson(tree);
    regex_t re;
    re.aut = aut;
    re.pattern = malloc((strlen(pattern) + 1) *sizeof(char));
    strcpy(re.pattern, pattern);
    return re;
}

void regex_free(regex_t re)
{
    automaton_free(re.aut);
    free(re.pattern);
}

int match(regex_t re, char* str)
{
    return match_nfa(re.aut, str);
}
