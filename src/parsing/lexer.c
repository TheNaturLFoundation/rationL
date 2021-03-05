#include "lexer.h"
#include <err.h>
#include <stdio.h>

static int is_group_last(const char *string)
{
    return *string == 0 || *string == ']';
}

static int get_group_range(const char **string, char *lower, char *upper)
{
    if (is_group_last(*string + 1) || (*string)[1] != '-' ||
        is_group_last(*string + 2))
        return 0;

    *lower = **string;
    *upper = (*string)[2];
    *string += 2;

    if (*upper < *lower)
        errx(EXIT_FAILURE, "invalid character range (%c-%c)", *lower, *upper);

    return 1;
}

static void add_range(Array *tokens, char lower, char upper)
{
    Token or_token = { .type = PUNCTUATION, .value = '|' };
    Token token;
    token.type = LITERAL;
    for (token.value = lower; token.value < upper; token.value++)
    {
        array_append(tokens, &token);
        array_append(tokens, &or_token);
    }
    array_append(tokens, &token);
}

static void add_literal(Array *tokens, char literal)
{
    const Token token = { .type = LITERAL, .value = literal };
    array_append(tokens, &token);
}

static void add_punctuation(Array *tokens, char punctuation)
{
    const Token token = { .type = PUNCTUATION, .value = punctuation };
    array_append(tokens, &token);
}

static void tokenize_group(const char **string, Array *tokens)
{
    const Token or_token = { .type = PUNCTUATION, .value = '|' };
    const Token par_token = { .type = PUNCTUATION, .value = '(' };

    array_append(tokens, &par_token);
    (*string)++;
    while (!is_group_last(*string))
    {
        char range_lower, range_upper;
        if (!is_group_last(*string) &&
            get_group_range(string, &range_lower, &range_upper))
            add_range(tokens, range_lower, range_upper);
        else
        {
            Token token = { .type = LITERAL, .value = **string };
            array_append(tokens, &token);
        }
        if (!is_group_last(++*string))
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
    for (token.value = ASCII_FIRST_CONTROL; token.value <= ASCII_LAST_CONTROL;
         token.value++)
    {
        array_append(tokens, &token);
        array_append(tokens, &or_token);
    }
    for (token.value = ASCII_FIRST_PRINTABLE; token.value < ASCII_LAST_PRINTABLE;
         token.value++)
    {
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
        int is_escapable = 0;
        Token token;
        Token par_token = { .type = PUNCTUATION, .value = '(' };

        char_switch:
        token.value = c;
        switch (c)
        {
        case '\\':
            if (!escaped)
            {
                switch (*(string + 1))
                {
                case 'd':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, '0', '9');
                    string++;
                    c = ')';
                    goto char_switch;
                case 'D':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, ASCII_FIRST_CONTROL, ASCII_LAST_CONTROL);
                    add_punctuation(tokens, '|');
                    add_range(tokens, ASCII_FIRST_PRINTABLE, '0' - 1);
                    add_punctuation(tokens, '|');
                    add_range(tokens, '9' + 1, ASCII_LAST_PRINTABLE);
                    string++;
                    c = ')';
                    goto char_switch;
                case 'w':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, 'a', 'z');
                    add_punctuation(tokens, '|');
                    add_range(tokens, 'A', 'Z');
                    add_punctuation(tokens, '|');
                    add_range(tokens, '0', '9');
                    add_punctuation(tokens, '|');
                    add_literal(tokens, '_');
                    string++;
                    c = ')';
                    goto char_switch;
                case 'W':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, ASCII_FIRST_CONTROL, ASCII_LAST_CONTROL);
                    add_punctuation(tokens, '|');
                    add_range(tokens, ASCII_FIRST_PRINTABLE, '0' - 1);
                    add_punctuation(tokens, '|');
                    add_range(tokens, '9' + 1, 'A' - 1);
                    add_punctuation(tokens, '|');
                    add_range(tokens, 'Z' + 1, '_' - 1);
                    add_punctuation(tokens, '|');
                    add_range(tokens, '_' + 1, 'a' - 1);
                    add_punctuation(tokens, '|');
                    add_range(tokens, 'z' + 1, ASCII_LAST_PRINTABLE);
                    string++;
                    c = ')';
                    goto char_switch;
                case 's':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_literal(tokens, ' ');
                    add_punctuation(tokens, '|');
                    add_range(tokens, ASCII_FIRST_CONTROL, ASCII_LAST_CONTROL);
                    string++;
                    c = ')';
                    goto char_switch;
                case 'S':
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, ' ' + 1, ASCII_LAST_PRINTABLE);
                    string++;
                    c = ')';
                    goto char_switch;
                }
                escaped = 1;
                continue;
            }
            is_escapable = 1;
        case '|':
            if (!escaped)
            {
                previous_concat = 0;
                token.type = PUNCTUATION;
                break;
            }
            is_escapable = 1;
        case '(':
            if (!escaped)
            {
                curr_concat = previous_concat;
                previous_concat = 0;
                token.type = PUNCTUATION;
                break;
            }
            is_escapable = 1;
        case '+':
        case '?':
        case '*':
        case ')':
            if (!escaped)
            {
                previous_concat = 1;
                token.type = PUNCTUATION;
                break;
            }
            is_escapable = 1;
        case '[':
            if (!escaped)
            {
                if (previous_concat)
                    array_append(tokens, &concat_token);
                previous_concat = 1;
                tokenize_group(&string, tokens);
                c = ')';
                goto char_switch; // Don't hurt me. Please.
                break;
            }
            is_escapable = 1;
        case '.':
            if (!escaped)
            {
                curr_concat = previous_concat;
                previous_concat = 1;
                tokenize_dot(tokens);
                string++;
                c = ')';
                goto char_switch;
                break;
            }
            is_escapable = 1;
        default:
            if (escaped && !is_escapable)
                errx(EXIT_FAILURE, "Can't escape character '%c'", c);
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
