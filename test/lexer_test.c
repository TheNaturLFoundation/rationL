#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "parsing/lexer.h"

void assert_eq_token(Token *actual, Token *expected)
{
    cr_assert_eq(actual->type, expected->type);
    cr_assert_eq(actual->value, expected->value);
}

Test(lexer, simple_single)
{
    char *regexp = "a";
    Array *tokens = tokenize(regexp);

    cr_assert_eq(tokens->size, 1);
    Token expected = { .type = LITERAL, .value = 'a' };
    assert_eq_token(array_get(tokens, 0), &expected);

    array_free(tokens);
}

Test(lexer, simple_operators)
{
    char *regexp = "ab|c*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '*' },
    };

    cr_assert_eq(tokens->size, 6);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, simple_punctuation)
{
    char *regexp = "(a+b)*|c*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '+' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '*' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '*' },
    };

    cr_assert_eq(tokens->size, 9);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, par_concat)
{
    char *regexp = "(a+b)(ad)*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '+' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'd' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '*' }
    };

    cr_assert_eq(tokens->size, 12);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, simple_group)
{
    char *regexp = "[abcd]";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'd' },
        { .type = PUNCTUATION, .value = ')' }
    };

    cr_assert_eq(tokens->size, 9);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, group_concat)
{
    char *regexp = "[ab](ab)(ab)";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
    };

    cr_assert_eq(tokens->size, 17);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, implicit_groups)
{
    char *regexp = "[ac-e2-5z]";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'd' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'e' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = '2' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = '3' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = '4' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = '5' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'z' },
        { .type = PUNCTUATION, .value = ')' }
    };

    for (size_t i = 0; i < tokens->size; i++)
        printf("%c", ((Token *)array_get(tokens, i))->value);
    puts("");
    printf("size = %zu\n", tokens->size);
    cr_assert_eq(tokens->size, 19);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}
