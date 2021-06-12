#include <criterion/criterion.h>
#include "rationl.h"

#define assert_match_eq(expected, actual) \
    assert_match_eq_(expected, actual, __LINE__)

static void assert_match_eq_(match *expected, match *actual, size_t line)
{
    if (expected == NULL || actual == NULL)
    {
        cr_assert_eq(expected, actual, "expected %p, got %p (line %zu)",
                     expected, actual, line);
        return;
    }

    cr_assert_str_eq(expected->string, actual->string,
                     "match string: expected '%s', got '%s' (line %zu)",
                     expected->string, actual->string, line);
    cr_assert_eq(expected->start, actual->start,
                 "match start: expected %zu, got %zu (line %zu)",
                 expected->start, actual->start, line);
    cr_assert_eq(expected->length, actual->length,
                 "match length: expected %zu, got %zu (line %zu)",
                 expected->length, actual->length, line);
    cr_assert_eq(expected->nb_groups, actual->nb_groups,
                 "match nb_groups: expected %zu, got %zu (line %zu)",
                 expected->nb_groups, actual->nb_groups, line);

    if (expected->groups == NULL || actual->groups == NULL)
        cr_assert_eq(expected->groups, actual->groups);

    for (size_t i = 0; i < expected->nb_groups; i++)
    {
        cr_assert_eq(expected->groups[i], actual->groups[i],
                     "match groups at pos %zu: expected '%s', got '%s' (line %zu)",
                     i, expected->groups[i], actual->groups[i], line);
    }
}

Test(interface_test, compile)
{
    reg_t regexp = regex_compile("aa*");
    cr_assert_neq(regexp.aut, NULL);
    regex_free(regexp);
}

Test(interface_test, read_daut)
{
    reg_t regexp = regex_read_daut(TEST_PATH "automaton/sum_mod_3.daut");
    cr_assert_str_eq(regexp.pattern, "(?:0*|0*10*1(?:(?:0|10*10*1))*10*)");
    regex_free(regexp);
}

Test(interface_test, match_email)
{
    reg_t email = regex_compile("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");

    match *match;

    match = regex_match(email, "rostan.tabet@gmail.com");
    cr_assert(match);
    match_free(match);

    match = regex_match(email, "rostan.tabet@gmail.fr.edu");
    cr_assert(match);
    match_free(match);

    match = regex_match(email, ".tabet@gmail.com");
    cr_assert_not(match);
    match_free(match);

    match = regex_match(email, "rostan.tabet@gmail.c");
    cr_assert_not(match);
    match_free(match);

    match = regex_match(email, "");
    cr_assert_not(match);
    match_free(match);

    regex_free(email);
}

Test(interface_test, search_numbers)
{
    reg_t int_reg = regex_compile("\\d+");

    match **matches = NULL;
    size_t n = regex_search(int_reg, "M3h... iT 41n't s0 7rIckY.", &matches);

    cr_assert_eq(n, 4, "expected 4, got %zu", n);

    match expected = {
        .string = "M3h... iT 41n't s0 7rIckY.",
        .nb_groups = 0,
        .groups = NULL,
    };

    //printf("match 1: '%.*s'\n", (int)matches[0]->length,
    //       matches[0]->string + matches[0]->start);

    expected.start = 1;
    expected.length = 1;
    assert_match_eq(&expected, matches[0]);

    expected.start = 10;
    expected.length = 2;
    assert_match_eq(&expected, matches[1]);

    expected.start = 17;
    expected.length = 1;
    assert_match_eq(&expected, matches[2]);

    expected.start = 19;
    expected.length = 1;
    assert_match_eq(&expected, matches[3]);

    for (size_t i = 0; i < n; i++)
        match_free(matches[i]);
    free(matches);

    regex_free(int_reg);
}

Test(interface_replace, replace_whitespace)
{
    reg_t whitespace_reg = regex_compile("\\s+");

    char *str = regex_sub(
        whitespace_reg, "Bonjour,\n  je  \t \n   m'appelle  \t   Rostan!", " ");
    cr_assert_str_eq(str, "Bonjour, je m'appelle Rostan!");

    free(str);
    regex_free(whitespace_reg);
}
