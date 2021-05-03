#include <stddef.h>
#include "automaton/automaton.h"

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

reg_t regex_compile(char* pattern);

reg_t regex_read_daut(char *path);

match *regex_match(reg_t re, char* str);

size_t regex_search(reg_t re, char *str, match **groups[]);

char *regex_sub(reg_t re, char *str, char *sub);

void regex_free(reg_t re);

void match_free(match *match);
