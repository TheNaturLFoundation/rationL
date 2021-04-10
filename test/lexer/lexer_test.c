#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "parsing/lexer.h"
#include <stdio.h>

#define assert_eq_token(actual, expected)                                      \
    assert_eq_token_(actual, expected, __LINE__)

void assert_eq_token_(Token *actual, Token *expected, size_t line)
{
    cr_assert_eq(actual->type, expected->type, "expected %s, got %s (line %zu).",
                 expected->type == PUNCTUATION ? "PUNCTUATION" : "LITERAL",
                 actual->type == PUNCTUATION ? "PUNCTUATION" : "LITERAL",
                 line);
    cr_assert_eq(actual->value, expected->value, "expected '%c', got '%c' (line %zu).",
                 expected->value, actual->value, line);
}
// NC = (), C = {}
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
    Token expected = Literal('a');
    assert_eq_token(array_get(tokens, 0), &expected);

    array_free(tokens);
}

Test(lexer, simple_operators)
{
    char *regexp = "ab|c*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('|'),
        Literal('c'),
        Punctuation('*'),
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
        Punctuation('{'),
        Literal('a'),
        Punctuation('+'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('*'),
        Punctuation('|'),
        Literal('c'),
        Punctuation('*'),
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
        Punctuation('{'),
        Literal('a'),
        Punctuation('+'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('.'),
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('d'),
        Punctuation('}'),
        Punctuation('*')
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
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('b'),
        Punctuation('|'),
        Literal('c'),
        Punctuation('|'),
        Literal('d'),
        Punctuation(')')
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
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('.'),
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
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
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('c'),
        Punctuation('|'),
        Literal('d'),
        Punctuation('|'),
        Literal('e'),
        Punctuation('|'),
        Literal('2'),
        Punctuation('|'),
        Literal('3'),
        Punctuation('|'),
        Literal('4'),
        Punctuation('|'),
        Literal('5'),
        Punctuation('|'),
        Literal('z'),
        Punctuation(')')
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

    Token or_token = Punctuation('|');
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
        Literal('\\'),
        Punctuation('.'),
        Literal('('),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('+'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('*'),
        Punctuation('.'),
        Literal(')'),
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

    Token or_token = Punctuation('|');
    Token concat_token = Punctuation('.');
    Token open_par_token = Punctuation('(');
    Token close_par_token = Punctuation(')');

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

    Token or_token = Punctuation('|');
    Token concat_token = Punctuation('.');
    Token open_par_token = Punctuation('(');
    Token close_par_token = Punctuation(')');

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

    Token or_token = Punctuation('|');
    Token concat_token = Punctuation('.');
    Token open_par_token = Punctuation('(');
    Token close_par_token = Punctuation(')');

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

Test(lexer, simple_repeat)
{
    char *regexp = "a{10}";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
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

Test(lexer, braces_repeat)
{
    char *regexp = "a{2}[ab]{1,3}(ab){2,}";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('?'),
        Punctuation('.'),
        Punctuation('('),
        Literal('a'),
        Punctuation('|'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('?'),
        Punctuation('.'),
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('.'),
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('+')
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
        Literal('a'),
        Punctuation('.'),
        Literal('{'),
        Punctuation('.'),
        Literal('2'),
        Punctuation('.'),
        Literal('-'),
        Punctuation('.'),
        Literal('3'),
        Punctuation('.'),
        Literal('}'),
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
        Literal('a'),
        Punctuation('.'),
        Literal('{'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Literal('{'),
        Punctuation('.'),
        Literal('1'),
        Punctuation('.'),
        Literal(','),
        Punctuation('.'),
        Literal('2'),
        Punctuation('.'),
        Literal('a'),
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

Test(lexer, non_capturing_group)
{
    char *regexp = "(?:a+b)a(?:ab)*";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        Punctuation('('),
        Literal('a'),
        Punctuation('+'),
        Punctuation('.'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Punctuation('('),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation(')'),
        Punctuation('*')
    };

    cr_assert_eq(tokens->size, 15);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}

Test(lexer, nested_groups)
{
    char *regexp = "((?:(a)))";
    Array *tokens = tokenize(regexp);

    Token expected_tokens[] = {
        Punctuation('{'),
        Punctuation('('),
        Punctuation('{'),
        Literal('a'),
        Punctuation('}'),
        Punctuation(')'),
        Punctuation('}'),
    };

    cr_assert_eq(tokens->size, 7);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(tokens);
}
