#include "lexer.h"
#include <stdio.h>

Array *tokenize(char *string)
{
    Array *tokens = Array(Token);

    for (char c = *string; c != 0; c = *++string)
    {
        Token token;
        token.value = c;
        switch (c)
        {
        case '*':
        case '|':
        case '+':
        case '?':
            token.type = PUNCTUATION;
            break;
        default:
            token.type = LITERAL;
        }

        array_append(tokens, &token);
    }

    return tokens;
}
