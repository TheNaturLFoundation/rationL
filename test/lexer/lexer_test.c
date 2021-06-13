#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "datatypes/array.h"
#include "parsing/lexer.h"
#include "parsing/parsing.h"
#include <stdio.h>

#define assert_eq_token(actual, expected)                                      \
    assert_eq_token_(actual, expected, __LINE__)

Token generate_group_token(char* string)
{
    Token tok;
    tok.type = CLASS;
    tok.value.letters = Array(char);
    while(*string)
    {
        array_append(tok.value.letters, &(*string));
        string++;
    }
    return tok;
}

void assert_eq_class(Array* actual, Array* expected)
{
    cr_assert_eq(actual->size, expected->size,
                 "expected a caracter class of size %zu but got a size of %zu",
                 expected->size, actual->size);
    for(size_t i = 0; i < actual->size; i++)
    {
        char actual_char = *(char*) array_get(actual, i);
        char expected_char = *(char*) array_get(expected, i);
        cr_assert_eq(actual_char, expected_char,
                 "expected a caracter %c but got %c", actual_char, expected_char);
    }
}

void assert_eq_token_(Token *actual, Token *expected, size_t line)
{
    cr_assert_eq(actual->type, expected->type, "expected %s, got %s (line %zu).",
                 expected->type == PUNCTUATION ? "PUNCTUATION" : expected->type == LITERAL ? "LITERAL" : "CLASS",
                 actual->type == PUNCTUATION ? "PUNCTUATION" : expected->type == LITERAL ? "LITERAL" : "CLASS",
                 line);
    if(actual->type == LITERAL || actual->type == PUNCTUATION)
        cr_assert_eq(actual->value.letter, expected->value.letter, "expected'%c', got '%c' (line %zu).",
                     expected->value.letter, actual->value.letter, line);
    else
        assert_eq_class(actual->value.letters, expected->value.letters);
}

void print_class(Array* letters)
{
    printf("CLASS(");
    arr_foreach(char, letter, letters)
    {
        printf("%c", letter);
    }
    printf(")");
}

// NC = (), C = {}
void print_tokens(Array *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token tok = * (Token*) array_get(tokens, i);
        if(tok.type == PUNCTUATION || tok.type == LITERAL)
            printf("%c", tok.value.letter);
        else
            print_class(tok.value.letters);
    }
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
    Token expected = generate_group_token("abcd");

    cr_assert_eq(tokens->size, 3, "expected %zu", tokens->size);

    assert_eq_token((Token*)array_get(tokens, 0), &(Token)Punctuation('('));
    assert_eq_token((Token*)array_get(tokens, 1), &expected);
    assert_eq_token((Token*)array_get(tokens, 2), &(Token)Punctuation(')'));

    array_free(expected.value.letters);

    free_tokens(tokens);
}

Test(lexer, group_concat)
{
    char *regexp = "(ab)[ab](ab)";
    Array *tokens = tokenize(regexp);
    Token ab_group = generate_group_token("ab");
    Token expected_tokens[] = {
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
        Punctuation('.'),
        Punctuation('('),
        ab_group,
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('{'),
        Literal('a'),
        Punctuation('.'),
        Literal('b'),
        Punctuation('}'),
    };

    cr_assert_eq(tokens->size, 15);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(ab_group.value.letters);

    free_tokens(tokens);
}

Test(lexer, implicit_groups)
{
    char *regexp = "[ac-e2-5z]";
    Array *tokens = tokenize(regexp);
    Token expected_group = generate_group_token("acde2345z");
    Token expected_tokens[] = {
        Punctuation('('),
        expected_group,
        Punctuation(')')
    };

    cr_assert_eq(tokens->size, 3);
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(expected_group.value.letters);
    free_tokens(tokens);
}

Test(lexer, simple_dot)
{
    char *regexp = ".";
    Array *tokens = tokenize(regexp);
    cr_assert_eq(tokens->size, 3);

    Token token;
    token.type = CLASS;
    token.value.letters = Array(char);
    for (char c = ASCII_FIRST_CONTROL; c <= ASCII_LAST_CONTROL;
         c++)
    {
        array_append(token.value.letters, &c);
    }
    for (char c = ASCII_FIRST_PRINTABLE; c <= ASCII_LAST_PRINTABLE;
         c++)
    {
        array_append(token.value.letters, &c);
    }
    Token expected_tokens[] = {
        Punctuation('('),
        token,
        Punctuation(')')
    };
    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }
    array_free(token.value.letters);
    free_tokens(tokens);
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

    cr_assert_eq(tokens->size, 7);

    Token little_d = generate_group_token("0123456789");

    Token big_d;
    big_d.type = CLASS;
    big_d.value.letters = Array(char);

    for (char c = ASCII_FIRST_CONTROL; c <= ASCII_LAST_CONTROL;
         c++)
    {
        array_append(big_d.value.letters, &c);
    }
    for (char c = ASCII_FIRST_PRINTABLE; c <= '0' - 1;
         c++)
    {
        array_append(big_d.value.letters, &c);
    }
    for (char c = '9' + 1; c <= ASCII_LAST_PRINTABLE;
         c++)
    {
        array_append(big_d.value.letters, &c);
    }

    Token expected_tokens[] = {
        Punctuation('('),
        little_d,
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('('),
        big_d,
        Punctuation(')'),
    };

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(little_d.value.letters);

    array_free(big_d.value.letters);

    free_tokens(tokens);
}

Test(lexer, word_special)
{
    char *regexp = "\\w\\W";  // \w <=> [a-zA-Z0-9_]
    Array *tokens = tokenize(regexp);
    Token little_w = generate_group_token(
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    Token big_w;
    big_w.type = CLASS;
    big_w.value.letters = Array(char);
    for (char c = ASCII_FIRST_CONTROL; c <= ASCII_LAST_CONTROL;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    for (char c = ASCII_FIRST_PRINTABLE; c <= '0' - 1;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    for (char c = '9' + 1; c <= 'A' - 1;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    for (char c = 'Z' + 1; c <= '_' - 1;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    for (char c = '_' + 1; c <= 'a' - 1;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    for (char c = 'z' + 1; c <= ASCII_LAST_PRINTABLE;
         c++)
    {
        array_append(big_w.value.letters, &c);
    }
    Token expected_tokens[] = {
        Punctuation('('),
        little_w,
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('('),
        big_w,
        Punctuation(')'),
    };

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(little_w.value.letters);

    array_free(big_w.value.letters);

    free_tokens(tokens);
}

Test(lexer, space_special)
{
    char *regexp = "\\s\\S";  // \s <=> [ \n\r\t\f]
    Array *tokens = tokenize(regexp);

    cr_assert_eq(tokens->size, 7);

    Token little_s = generate_group_token(
        " \t\n\v\f\r");

    Token big_s;
    big_s.type = CLASS;
    big_s.value.letters = Array(char);

    // First printable caracter is ' '
    for (char c = ' ' + 1; c <= ASCII_LAST_PRINTABLE;
         c++)
    {
        array_append(big_s.value.letters, &c);
    }

    Token expected_tokens[] = {
        Punctuation('('),
        little_s,
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('('),
        big_s,
        Punctuation(')'),
    };

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(little_s.value.letters);

    array_free(big_s.value.letters);

    free_tokens(tokens);
}

Test(lexer, special_concat)
{
    char *regexp = "a\\d\\w\\s.";
    Array *tokens = tokenize(regexp);
    size_t expected = 17;
    // Only test the size, trust actual values according to previous tests
    cr_assert_eq(tokens->size, expected, "expected %zu, got %zu", expected, tokens->size);

    free_tokens(tokens);
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

Test(lexer, repeat_free)
{
    char *regexp = "[ab]{3}";
    Array *tokens = tokenize(regexp);
    free_tokens(tokens);
}

Test(lexer, braces_repeat)
{
    char *regexp = "a{2}[ab]{1,3}(ab){2,}";
    Array *tokens = tokenize(regexp);

    Token ab = generate_group_token("ab");

    Token expected_tokens[] = {
        Literal('a'),
        Punctuation('.'),
        Literal('a'),
        Punctuation('.'),
        Punctuation('('),
        ab,
        Punctuation(')'),
        Punctuation('.'),
        Punctuation('('),
        ab,
        Punctuation(')'),
        Punctuation('?'),
        Punctuation('.'),
        Punctuation('('),
        ab,
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

    cr_assert_eq(tokens->size, 30);

    for (size_t i = 0; i < tokens->size; i++)
    {
        Token *actual = array_get(tokens, i);
        Token expected = expected_tokens[i];
        assert_eq_token(actual, &expected);
    }

    array_free(ab.value.letters);
    free_tokens(tokens);
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
