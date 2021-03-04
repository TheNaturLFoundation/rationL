#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "parsing/lexer.h"

void assert_eq_token(Token *actual, Token *expected)
{
    cr_assert_eq(actual->type, expected->type);
    cr_assert_eq(actual->value, expected->value);
}

Test(lexer, simple_literal)
{
    char *regexp = "abc";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = LITERAL, .value = 'b' },
        { .type = LITERAL, .value = 'c' },
    };

    for (size_t i = 0; i < 3; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, simple)
{
    char *regexp = "ab|c*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '*' },
    };

    for (size_t i = 0; i < 5; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}
