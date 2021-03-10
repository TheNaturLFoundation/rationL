#include "lexer.h"
#include "datatypes/linked_list.h"
#include "datatypes/array.h"
#include <err.h>
#include <stdio.h>

struct scope
{
    /**
     * Either '(' or '['.
     */
    char start_char;
    size_t start_index;
    /**
     * -1 if undefined.
     */
    ssize_t end_index;
};

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

/**
 * Returns the unsigned integer pointed by the string.
 * If there is no integer, return -1.
 */
static ssize_t get_uint(const char **string)
{
    if (**string < '0' || **string > '9')
        return -1;

    ssize_t total = 0;
    while (**string >= '0' && **string <= '9')
    {
        total += **string - '0';
        (*string)++;
    }

    return total;
}

/**
 * Repeat a group of tokens based on the repetition pattern inside curly braces.
 * Move the string pointer at the end of the curly braces pattern.
 * If the pattern is incorrect, return 0, keep the string pointer unchanged.
 */
static int tokenize_repetition(const char **string, Array *tokens,
                               size_t start_index, size_t end_index)
{
    const char *orig = (*string)++;
    ssize_t lower = get_uint(string);

    if (lower == -1)
    {
        *string = orig;
        return 0;
    }

    ssize_t upper = -1;
    int comma = 0;
    if (**string != '}')
    {
        if (*(*string)++ != ',')
        {
            *string = orig;
            return 0;
        }
        comma = 1;
        upper = get_uint(string);
        if (**string != '}')
        {
            *string = orig;
            return 0;
        }
    }

    if (upper != -1 && upper < lower)
        errx(EXIT_FAILURE, "min repeat greater than max repeat");

    Array *range = array_sub(tokens, start_index, end_index);
    for (size_t i = 0; i < lower - 1; i++)
    {
        add_punctuation(tokens, '.');
        array_concat(tokens, range);
    }

    if (!comma)
    {
        array_free(range);
        return 1;
    }

    if (upper == -1)
        add_punctuation(tokens, '+');
    else
    {
        add_punctuation(range, '?');
        for (size_t i = 0; i < upper - lower; i++)
        {
            add_punctuation(tokens, '.');
            array_concat(tokens, range);
        }
    }

    array_free(range);
    return 1;
}

// Assumes that the string ends with 0
Array *tokenize(const char *string)
{
    Array *tokens = Array(Token);
    const Token concat_token = { .type = PUNCTUATION, .value = '.' };
    // True if the previous character is implicitly concatenated to a literal.
    int previous_concat = 0;
    // True if the previous character was an escaping '\'.
    int escaped = 0;

    // Stack of scopes.
    LinkedList *scopes = LinkedList(struct scope);
    struct scope last_scope;
    last_scope.end_index = -1;

    for (char c = *string; c != 0; c = *++string)
    {
        int curr_concat = 0;
        int is_escapable = 0;
        int false_par = 0;
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
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, '0', '9');
                    string++;
                    c = ')';
                    goto char_switch;
                }
                case 'D':
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
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
                }
                case 'w':
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
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
                }
                case 'W':
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
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
                }
                case 's':
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
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
                }
                case 'S':
                {
                    struct scope scope = {
                        .start_char = '[',
                        .start_index = tokens->size + previous_concat,
                        .end_index = -1
                    };
                    list_push_back(scopes, &scope);
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    array_append(tokens, &par_token);
                    add_range(tokens, ' ' + 1, ASCII_LAST_PRINTABLE);
                    string++;
                    c = ')';
                    goto char_switch;
                }
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
        {
            if (!escaped && !false_par)
            {
                struct scope scope = {
                    .start_char = '[',
                    .start_index = tokens->size + previous_concat,
                    .end_index = -1
                };
                list_push_back(scopes, &scope);
                curr_concat = previous_concat;
                previous_concat = 0;
                token.type = PUNCTUATION;
                break;
            }
            is_escapable = 1;
        }
        case ')':
        {
            if (!escaped)
            {
                if (scopes->next == NULL)
                    errx(EXIT_FAILURE, "parenthesis not balanced");
                LinkedList *last_scope_node = list_pop(scopes);
                last_scope = *(struct scope *)last_scope_node->data;
                last_scope.end_index = tokens->size;
                list_free(last_scope_node);
            }
        }
        case '+':
        case '?':
        case '*':
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
                struct scope scope = {
                    .start_char = '[',
                    .start_index = tokens->size + previous_concat,
                    .end_index = -1
                };
                list_push_back(scopes, &scope);
                if (previous_concat)
                    array_append(tokens, &concat_token);
                previous_concat = 1;
                tokenize_group(&string, tokens);
                c = ')';
                goto char_switch; // Don't hurt me. Please.
                break;
            }
            is_escapable = 1;
        case ']':
            if (!escaped)
                errx(EXIT_FAILURE, "unbalanced brackets");
            is_escapable = 1;
        case '.':
            if (!escaped)
            {
                struct scope scope = {
                    .start_char = '(',
                    .start_index = tokens->size + previous_concat,
                    .end_index = -1
                };
                list_push_back(scopes, &scope);
                if (previous_concat)
                    array_append(tokens, &concat_token);
                previous_concat = 1;
                tokenize_dot(tokens);
                c = ')';
                goto char_switch;
            }
            is_escapable = 1;
        case '{':
            if (!escaped && tokens->size != 0)
            {
                size_t upper = tokens->size - 1;
                size_t lower = upper;
                if (last_scope.end_index == upper)
                    lower = last_scope.start_index;
                if (tokenize_repetition(&string, tokens, lower, upper))
                    continue;
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

    list_free(scopes);
    return tokens;
}
