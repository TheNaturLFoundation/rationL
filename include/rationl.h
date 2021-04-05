typedef struct Automaton Automaton;

typedef struct regex_t
{
    Automaton* aut;
    char* pattern;
} regex_t;

regex_t regex_compile(char* pattern);

int regex_match(regex_t re, char* str);

void regex_free(regex_t re);
