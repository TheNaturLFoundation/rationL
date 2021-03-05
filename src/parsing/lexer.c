#include "lexer.h"
#include <err.h>
#include <stdio.h>

static void tokenize_group(const char **string, Array *tokens)
{
    const Token or_token = { .type = PUNCTUATION, .value = '|' };
    const Token par_token = { .type = PUNCTUATION, .value = '(' };

    array_append(tokens, &par_token);
    (*string)++;
    int is_last = **string == 0 || **string == ']';
    while (!is_last)
    {
        char curr = **string;
        Token token = { .type = LITERAL, .value = curr };
        (*string)++;
        is_last = **string == 0 || **string == ']';

        array_append(tokens, &token);
        if (!is_last && **string == '-' &&
            (*string)[1] != 0 && (*string)[1] != ']')
        {
            char next = (*string)[1];
            if (**string == '-' && next != 0 && next != ']')
            {
                if (next < curr)
                    errx(EXIT_FAILURE, "invalid character range");
                for (++curr; curr <= next; ++curr)
                {
                    array_append(tokens, &or_token);
                    Token t = { .type = LITERAL, .value = curr };
                    array_append(tokens, &t);
                }
                *string += 2;
            }
        }
        is_last = **string == 0 || **string == ']';
        if (!is_last)
            array_append(tokens, &or_token);
    }

    if (*string == 0)
        errx(EXIT_FAILURE, "brackets not balanced");
}

static void tokenize_dot(Array *tokens)
{
    const Token or_token = { .type = PUNCTUATION, .value = '|' };
    const Token par_token = { .type = PUNCTUATION, .value = '(' };

    array_append(tokens, &par_token);
    Token token;
    token.type = LITERAL;
    for (char c = ASCII_FIRST_PRINTABLE; c < ASCII_LAST_PRINTABLE; c++)
    {
        token.value = c;
        array_append(tokens, &token);
        array_append(tokens, &or_token);
    }
    token.value = ASCII_LAST_PRINTABLE;
    array_append(tokens, &token);
}

// Assumes that the string ends with 0
Array *tokenize(const char *string)
{
    Array *tokens = Array(Token);
    const Token concat_token = { .type = PUNCTUATION, .value = '.' };
    // True if the previous character is implicitly concatenated to a literal
    int previous_concat = 0;
    // True if the previous character was an escaping '\'
    int escaped = 0;

    for (char c = *string; c != 0; c = *++string)
    {
        int curr_concat = 0;
        Token token;

        char_switch:
        token.value = c;
        switch (c)
        {
        case '\\':
            escaped = 1;
            continue;
        case '*':
        case '|':
        case '+':
        case '?':
            previous_concat = 0;
            token.type = PUNCTUATION;
            break;
        case '(':
            curr_concat = previous_concat;
            previous_concat = 0;
            token.type = PUNCTUATION;
            break;
        case ')':
            previous_concat = 1;
            token.type = PUNCTUATION;
            break;
        case '[':
            curr_concat = previous_concat;
            tokenize_group(&string, tokens);
            c = ')';
            goto char_switch;  // Don't hurt me. Please.
            break;
        case '.':
            curr_concat = previous_concat;
            previous_concat = 1;
            tokenize_dot(tokens);
            string++;
            c = ')';
            goto char_switch;
            break;
        default:
            curr_concat = previous_concat;
            previous_concat = 1;
            token.type = LITERAL;
            break;
        }
        escaped = 0;

        if (curr_concat)
            array_append(tokens, &concat_token);
        array_append(tokens, &token);
    }

    return tokens;
}
