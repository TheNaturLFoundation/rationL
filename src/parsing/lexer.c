#include "parsing/lexer.h"

#include <err.h>
#include <stdio.h>

#include "datatypes/array.h"
#include "datatypes/linked_list.h"

static int is_group_last(const char *string)
{
    return *string == 0 || *string == ']';
}

static int get_group_range(const char **string, char *lower, char *upper)
{
    if (is_group_last(*string + 1) || (*string)[1] != '-'
        || is_group_last(*string + 2))
        return 0;

    *lower = **string;
    *upper = (*string)[2];
    *string += 2;

    if (*upper < *lower)
        errx(EXIT_FAILURE, "invalid character range (%c-%c)", *lower,
             *upper); // LCOV_EXCL_LINE

    return 1;
}

/*
 * Carefully frees the token array to avoid memory leaks :D
 */
void free_tokens(Array *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *tok = array_get(tokens, i);
        if (tok->type == CLASS)
        {
            if (tok->value.letters != NULL)
            {
                array_free(tok->value.letters);
            }
        }
    }
    array_free(tokens);
}

/*
 * This function add the required caracters to the letters array
 * which then will be the letters field of a token...
 */
static void add_range(Array *letters, char lower, char upper)
{
    for (char c = lower; c <= upper; c++)
    {
        array_append(letters, &c);
    }
}

static void add_range_except(Array *letters, char lower, char upper)
{
    for (int c = ASCII_FIRST_PRINTABLE; c < lower; c++)
    {
        array_append(letters, &c);
    }
    for (int c = upper+1; c <= ASCII_LAST_PRINTABLE; c++)
    {
        array_append(letters, &c);
    }
}


static void add_punctuation(Array *tokens, char punctuation)
{
    const Token token = Punctuation(punctuation);
    array_append(tokens, &token);
}

static void tokenize_group(const char **string, Array *tokens)
{
    Array *letters = Array(Letter);
    const Token par_token = Punctuation('(');
    array_append(tokens, &par_token);
    Token tok;
    tok.type = CLASS;
    (*string)++;

    if (**string == '^')
    {
        (*string)++;
        while(!is_group_last(*string))
        {
            char range_lower, range_upper;
            if (!is_group_last(*string)
                && get_group_range(string, &range_lower, &range_upper))
                add_range_except(letters, range_lower, range_upper);
            else
            {
                add_range_except(letters, **string, *(*string+2));
                *string +=2;
            }
            (*string)++;
        }
    }
    else
    {
        while (!is_group_last(*string))
        {
            char range_lower, range_upper;
            if (!is_group_last(*string)
                && get_group_range(string, &range_lower, &range_upper))
                add_range(letters, range_lower, range_upper);
            else
            {
                array_append(letters, &(**string));
            }
            (*string)++;
        }
    }
    tok.value.letters = letters;
    array_append(tokens, &tok);
    if (*string == 0)
        errx(EXIT_FAILURE, "brackets not balanced"); // LCOV_EXCL_LINE
}

static void tokenize_dot(Array *tokens)
{
    const Token par_token = Punctuation('(');
    array_append(tokens, &par_token);
    Token token;
    token.type = CLASS;
    Array *letters = Array(Letter);
    for (char c = ASCII_FIRST_CONTROL; c <= ASCII_LAST_CONTROL; c++)
    {
        array_append(letters, &c);
    }
    for (char c = ASCII_FIRST_PRINTABLE; c <= ASCII_LAST_PRINTABLE; c++)
    {
        array_append(letters, &c);
    }
    token.value.letters = letters;
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
        total *= 10;
        total += **string - '0';
        (*string)++;
    }

    return total;
}

void array_concat_copy_tkns(Array *dst, Array *src)
{
    for (size_t j = 0; j < src->size; j++)
    {
        Token *tk = array_get(src, j);
        if (tk->type == CLASS)
        {
            Token cpy;
            cpy.type = tk->type;
            cpy.value.letters = Array(Letter);
            for (size_t i = 0; i < tk->value.letters->size; i++)
            {
                Letter let = *(Letter *)array_get(tk->value.letters, i);
                array_append(cpy.value.letters, &let);
            }
            array_append(dst, &cpy);
        }
        else
        {
            array_append(dst, tk);
        }
    }
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
        errx(EXIT_FAILURE,
             "min repeat greater than max repeat"); // LCOV_EXCL_LINE

    Array *range = array_sub(tokens, start_index, end_index);
    for (ssize_t i = 0; i < lower - 1; i++)
    {
        add_punctuation(tokens, '.');
        array_concat_copy_tkns(tokens, range);
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
        for (ssize_t i = 0; i < upper - lower; i++)
        {
            add_punctuation(tokens, '.');
            array_concat_copy_tkns(tokens, range);
        }
    }

    array_free(range);
    return 1;
}

// Assumes that the string ends with 0
Array *tokenize(const char *string)
{
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

    char isString = 1;

    Array *tokens = Array(Token);
    const Token concat_token = Punctuation('.');
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
        Token token;
        Token par_token = Punctuation('(');

    char_switch:
        token.value.letter = c;
        switch (c)
        {
            case '\\':
                if (!escaped)
                {
                    isString = 0;
                    switch (*(string + 1))
                    {
                        case 'd': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;
                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            add_range(class.value.letters, '0', '9');
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                        case 'D': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;
                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            add_range(class.value.letters, ASCII_FIRST_CONTROL,
                                      ASCII_LAST_CONTROL);
                            add_range(class.value.letters, ASCII_FIRST_PRINTABLE,
                                      '0' - 1);
                            add_range(class.value.letters, '9' + 1,
                                      ASCII_LAST_PRINTABLE);
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                        case 'w': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;
                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            add_range(class.value.letters, 'a', 'z');
                            add_range(class.value.letters, 'A', 'Z');
                            add_range(class.value.letters, '0', '9');
                            array_append(class.value.letters, &(char){ '_' });
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                        case 'W': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;
                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            add_range(class.value.letters, ASCII_FIRST_CONTROL,
                                      ASCII_LAST_CONTROL);
                            add_range(class.value.letters, ASCII_FIRST_PRINTABLE,
                                      '0' - 1);
                            add_range(class.value.letters, '9' + 1, 'A' - 1);
                            add_range(class.value.letters, 'Z' + 1, '_' - 1);
                            add_range(class.value.letters, '_' + 1, 'a' - 1);
                            add_range(class.value.letters, 'z' + 1,
                                      ASCII_LAST_PRINTABLE);
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                        case 's': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;
                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            array_append(class.value.letters, &(char){ ' ' });
                            add_range(class.value.letters, ASCII_FIRST_CONTROL,
                                      ASCII_LAST_CONTROL);
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                        case 'S': {
                            struct scope scope = { .start_char = '[',
                            .start_index =
                            tokens->size + previous_concat,
                            .end_index = -1 };
                            list_push_front(scopes, &scope);
                            if (previous_concat)
                                array_append(tokens, &concat_token);
                            previous_concat = 1;

                            array_append(tokens, &par_token);
                            Token class;
                            class.type = CLASS;
                            class.value.letters = Array(Letter);
                            // First printable caracter is ' ' so first no space is ' '
                            // + 1
                            add_range(class.value.letters, ' ' + 1,
                                      ASCII_LAST_PRINTABLE);
                            array_append(tokens, &class);
                            string++;
                            c = ')';
                            goto char_switch;
                        }
                    }
                    escaped = 1;
                    continue;
                }
                is_escapable = 1;
                /* fall through */
            case '|':
                if (!escaped)
                {
                    isString = 0;
                    previous_concat = 0;
                    token.type = PUNCTUATION;
                    break;
                }
                is_escapable = 1;
                /* fall through */
            case '(':
                {
                    if (!escaped)
                    {
                        isString = 0;
                        int capturing = *(string + 1) != '?' || *(string + 2) != ':';
                        struct scope scope = { .start_char = capturing ? '{' : '(',
                        .start_index =
                        tokens->size + previous_concat,
                        .end_index = -1 };
                        if (capturing)
                            token.value.letter = '{';
                        else
                            string += 2;
                        list_push_front(scopes, &scope);
                        curr_concat = previous_concat;
                        previous_concat = 0;
                        token.type = PUNCTUATION;
                        break;
                    }
                    is_escapable = 1;
                }
                /* fall through */
            case ')':
                {
                    if (!escaped)
                    {
                        isString = 0;
                        if (list_empty(scopes))
                            errx(EXIT_FAILURE,
                                 "parenthesis not balanced"); // LCOV_EXCL_LINE
                        LinkedList *last_scope_node = list_pop_front(scopes);
                        last_scope = *(struct scope *)last_scope_node->data;
                        last_scope.end_index = tokens->size;
                        if (last_scope.start_char == '{') // Capturing group
                            token.value.letter = '}';
                        list_free_from(last_scope_node);
                    }
                    is_escapable = 1;
                }
                /* fall through */
            case '+':
            case '?':
            case '*':
                if (!escaped)
                {
                    isString = 0;
                    previous_concat = 1;
                    token.type = PUNCTUATION;
                    break;
                }
                is_escapable = 1;
                /* fall through */
            case '[':
                if (!escaped)
                {
                    isString = 0;
                    struct scope scope = { .start_char = '[',
                    .start_index =
                    tokens->size + previous_concat,
                    .end_index = -1 };
                    list_push_front(scopes, &scope);
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    tokenize_group(&string, tokens);
                    c = ')';
                    goto char_switch; // Don't hurt me. Please.
                    break;
                }
                is_escapable = 1;
                /* fall through */
            case ']':
                if (!escaped)
                    errx(EXIT_FAILURE, "unbalanced brackets"); // LCOV_EXCL_LINE
                is_escapable = 1;
                /* fall through */
            case '.':
                if (!escaped)
                {
                    isString = 0;
                    struct scope scope = { .start_char = '[',
                    .start_index =
                    tokens->size + previous_concat,
                    .end_index = -1 };
                    list_push_front(scopes, &scope);
                    if (previous_concat)
                        array_append(tokens, &concat_token);
                    previous_concat = 1;
                    tokenize_dot(tokens);
                    c = ')';
                    goto char_switch;
                }
                is_escapable = 1;
                /* fall through */
            case '{':
                if (!escaped && tokens->size != 0)
                {
                    isString = 0;
                    ssize_t upper = tokens->size - 1;
                    ssize_t lower = upper;
                    if (last_scope.end_index == upper)
                        lower = last_scope.start_index;
                    if (tokenize_repetition(&string, tokens, lower, upper))
                        continue;
                }
                is_escapable = 1;
                /* fall through */
            default:
                if (escaped && !is_escapable)
                    errx(EXIT_FAILURE, "Can't escape character '%c'",
                         c); // LCOV_EXCL_LINE
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

    if(isString)
        return NULL;

    return tokens;
}
