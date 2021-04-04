#include "rationl/automaton.h"

typedef struct regex_t
{
    Automaton* aut;
    char* pattern;
} regex_t;

regex_t compile(char* pattern);

int match(regex_t re, char* str);
