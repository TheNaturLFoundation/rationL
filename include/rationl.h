#include <stddef.h>

typedef struct Automaton Automaton;

typedef struct reg_t
{
    Automaton* aut;
    char* pattern;
} reg_t;

reg_t regex_compile(char* pattern);

int regex_match(reg_t re, char* str);

size_t regex_search(reg_t re, char *str, char **groups[]);

char *regex_sub(reg_t re, char *str, char *sub);

void regex_free(reg_t re);
