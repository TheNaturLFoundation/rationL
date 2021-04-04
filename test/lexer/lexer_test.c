#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "rationl/lexer.h"
#include <stdio.h>

void assert_eq_token(Token *actual, Token *expected)
{
    cr_assert_eq(actual->type, expected->type, "expected %s, got %s.",
                 expected->type == PUNCTUATION ? "PUNCTUATION" : "LITERAL",
                 actual->type == PUNCTUATION ? "PUNCTUATION" : "LITERAL");
    cr_assert_eq(actual->value, expected->value, "expected '%c', got '%c'",
                 expected->value, actual->value);
}

void print_tokens(Array *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
        printf("%c", ((Token *)array_get(tokens, i))->value);
    puts("");
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
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '*' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'c' },
        { .type = PUNCTUATION, .value = '*' },
    };

    cr_assert_eq(tokens->size, 10);
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
        { .type = PUNCTUATION, .value = '.' },
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

    cr_assert_eq(tokens->size, 13);
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
    char *regexp = "(ab)[ab](ab)";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '.' },
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

    cr_assert_eq(tokens->size, 19);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, simple_dot)
{
    char *regexp = ".";
    Array *tokens = tokenize(regexp);

    size_t expected_size =
        (ASCII_LAST_CONTROL - ASCII_FIRST_CONTROL + 1 +
         ASCII_LAST_PRINTABLE - ASCII_FIRST_PRINTABLE + 1) * 2 + 1;
    cr_assert_eq(tokens->size, expected_size);
    cr_assert_eq(((Token *)array_get(tokens, 0))->value, '(');

    Token or_token = { .type = PUNCTUATION, .value = '|' };
    size_t i = 1;
    Token token;
    token.type = LITERAL;
    for (token.value = ASCII_FIRST_CONTROL; token.value <= ASCII_LAST_CONTROL;
         token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = ASCII_FIRST_PRINTABLE; token.value < ASCII_LAST_PRINTABLE;
         token.value++)
    {
        Token *actual = array_get(tokens, i++);
        assert_eq_token(actual, &token);

        Token *actual_or = array_get(tokens, i++);
        assert_eq_token(actual_or, &or_token);
    }
    assert_eq_token(array_get(tokens, i), &token);
    cr_assert_eq(((Token *)array_get(tokens, i + 1))->value, ')');

    array_free(tokens);
}

Test(lexer, escaping)
{
    char *regexp = "\\\\\\(a\\+b*\\)";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = '\\' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '(' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '+' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = '*' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = ')' },
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

Test(lexer, digit_special)
{
    char *regexp = "\\d\\D";
    Array *tokens = tokenize(regexp);

    size_t expected_size =
        (ASCII_LAST_CONTROL - ASCII_FIRST_CONTROL + 1 +
         ASCII_LAST_PRINTABLE - ASCII_FIRST_PRINTABLE + 1) * 2 + 3;
    cr_assert_eq(tokens->size, expected_size);

    Token or_token = { .type = PUNCTUATION, .value = '|' };
    Token concat_token = { .type = PUNCTUATION, .value = '.' };
    Token open_par_token = { .type = PUNCTUATION, .value = '(' };
    Token close_par_token = { .type = PUNCTUATION, .value = ')' };

    Token token;
    token.type = LITERAL;
    size_t i = 1;
    for (token.value = '0'; token.value < '9'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);
    assert_eq_token(array_get(tokens, i++), &concat_token);
    assert_eq_token(array_get(tokens, i++), &open_par_token);
    for (token.value = ASCII_FIRST_CONTROL; token.value <= ASCII_LAST_CONTROL; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = ASCII_FIRST_PRINTABLE; token.value < '0'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = '9' + 1; token.value < ASCII_LAST_PRINTABLE; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);

    array_free(tokens);
}

Test(lexer, word_special)
{
    char *regexp = "\\w\\W";  // \w <=> [a-zA-Z0-9_]
    Array *tokens = tokenize(regexp);

    size_t expected_size =
        (ASCII_LAST_CONTROL - ASCII_FIRST_CONTROL + 1 +
         ASCII_LAST_PRINTABLE - ASCII_FIRST_PRINTABLE + 1) * 2 + 3;
    cr_assert_eq(tokens->size, expected_size);

    Token or_token = { .type = PUNCTUATION, .value = '|' };
    Token concat_token = { .type = PUNCTUATION, .value = '.' };
    Token open_par_token = { .type = PUNCTUATION, .value = '(' };
    Token close_par_token = { .type = PUNCTUATION, .value = ')' };

    Token token;
    token.type = LITERAL;
    size_t i = 1;
    for (token.value = 'a'; token.value <= 'z'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = 'A'; token.value <= 'Z'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = '0'; token.value <= '9'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    token.value = '_';
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);
    assert_eq_token(array_get(tokens, i++), &concat_token);
    assert_eq_token(array_get(tokens, i++), &open_par_token);
    for (token.value = ASCII_FIRST_CONTROL; token.value <= ASCII_LAST_CONTROL; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = ASCII_FIRST_PRINTABLE; token.value < '0'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = '9' + 1; token.value < 'A'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = 'Z' + 1; token.value < '_'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = '_' + 1; token.value < 'a'; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = 'z' + 1; token.value < ASCII_LAST_PRINTABLE; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);

    array_free(tokens);
}

Test(lexer, space_special)
{
    char *regexp = "\\s\\S";  // \s <=> [ \n\r\t\f]
    Array *tokens = tokenize(regexp);

    size_t expected_size =
        (ASCII_LAST_CONTROL - ASCII_FIRST_CONTROL + 1 +
         ASCII_LAST_PRINTABLE - ASCII_FIRST_PRINTABLE + 1) * 2 + 3;
    cr_assert_eq(tokens->size, expected_size);

    Token or_token = { .type = PUNCTUATION, .value = '|' };
    Token concat_token = { .type = PUNCTUATION, .value = '.' };
    Token open_par_token = { .type = PUNCTUATION, .value = '(' };
    Token close_par_token = { .type = PUNCTUATION, .value = ')' };

    Token token;
    token.type = LITERAL;
    size_t i = 1;
    token.value = ' ';
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &or_token);
    for (token.value = ASCII_FIRST_CONTROL; token.value < ASCII_LAST_CONTROL; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);
    assert_eq_token(array_get(tokens, i++), &concat_token);
    assert_eq_token(array_get(tokens, i++), &open_par_token);
    for (token.value = ASCII_FIRST_PRINTABLE; token.value < ' '; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    for (token.value = ' ' + 1; token.value < ASCII_LAST_PRINTABLE; token.value++)
    {
        assert_eq_token(array_get(tokens, i++), &token);
        assert_eq_token(array_get(tokens, i++), &or_token);
    }
    assert_eq_token(array_get(tokens, i++), &token);
    assert_eq_token(array_get(tokens, i++), &close_par_token);

    array_free(tokens);
}

Test(lexer, special_concat)
{
    char *regexp = "a\\d\\w\\s.";
    Array *tokens = tokenize(regexp);

    // Only test the size, trust actual values according to previous tests
    size_t expected = ('9' - '0' + 1) * 2 + 2
        + ('z' - 'a' + 2 + 'Z' - 'A' + 2 + '9' - '0' + 1) * 2 + 2
        + (ASCII_LAST_CONTROL - ASCII_FIRST_CONTROL + 1) * 2 + 2
        + (ASCII_LAST_PRINTABLE - ASCII_FIRST_PRINTABLE + 1 + ASCII_LAST_CONTROL
           - ASCII_FIRST_CONTROL + 1)
            * 2 + 1
        + 2;
    cr_assert_eq(tokens->size, expected, "expected %zu, got %zu", expected, tokens->size);

    array_free(tokens);
}

Test(lexer, braces_repeat)
{
    char *regexp = "a{2}[ab]{1,3}(ab){2,}";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '?' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = PUNCTUATION, .value = '(' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '|' },
        { .type = LITERAL, .value = 'b' },
        { .type = PUNCTUATION, .value = ')' },
        { .type = PUNCTUATION, .value = '?' },
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
        { .type = PUNCTUATION, .value = '+' }
    };

    cr_assert_eq(tokens->size, 36);

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, braces_no_repeat)
{
    char *regexp = "a{2-3}";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '{' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '2' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '-' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '3' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '}' },
    };

    cr_assert_eq(tokens->size, 11);

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, braces_fake_repeat)
{
    char *regexp = "a{aa{1,2a";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '{' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'a' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '{' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '1' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = ',' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = '2' },
        { .type = PUNCTUATION, .value = '.' },
        { .type = LITERAL, .value = 'a' },
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
