#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include "automaton/automaton.h"
#include "automaton/determine.h"
#include "automaton/thompson.h"
#include "datatypes/array.h"
#include "datatypes/bin_tree.h"
#include "matching/matching.h"
#include "parsing/parsing.h"

#define assert_match_eq(expected, actual)                                      \
    assert_match_eq_(expected, actual, __LINE__)

static void assert_match_eq_(Match *expected, Match *actual, size_t line)
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
        cr_assert_eq(expected->groups, actual->groups,
                     "match groups: expected %snull, got %snull (line %zu)\n",
                     expected->groups == NULL ? "" : "non-",
                     actual->groups == NULL ? "" : "non-", line);

    for (size_t i = 0; i < expected->nb_groups; i++)
    {
        cr_assert_str_eq(
            expected->groups[i], actual->groups[i],
            "match groups at pos %zu: expected '%s', got '%s' (line %zu)", i,
            expected->groups[i], actual->groups[i], line);
    }
}

Test(matching, abba)
{
    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut", 6);
    Match *match;

    // abba
    match = match_nfa(abba, "abba");
    Match expected = { .string = "abba",
                       .start = 0,
                       .length = 4,
                       .nb_groups = 0,
                       .groups = NULL };
    assert_match_eq(&expected, match);
    free(match);

    // abbaa
    match = match_nfa(abba, "abbaa");
    expected.string = "abbaa";
    assert_match_eq(&expected, match);
    free(match);

    // abb
    match = match_nfa(abba, "abb");
    assert_match_eq(NULL, match);
    free(match);

    // empty string
    match = match_nfa(abba, "");
    assert_match_eq(NULL, match);
    free(match);

    automaton_free(abba);
}

Test(matching, a_or_b)
{
    Automaton *a_or_b = automaton_from_daut(TEST_PATH "automaton/a+b.daut", 3);
    Match *match;

    match = match_nfa(a_or_b, "a");
    Match expected = {
        .string = "a", .start = 0, .length = 1, .nb_groups = 0, .groups = NULL
    };
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(a_or_b, "abba");
    expected.string = "abba";
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(a_or_b, "b");
    expected.string = "b";
    assert_match_eq(&expected, match);
    free(match);

    assert_match_eq(NULL, match_nfa(a_or_b, "c"));
    assert_match_eq(NULL, match_nfa(a_or_b, "dba"));
    assert_match_eq(NULL, match_nfa(a_or_b, ""));

    automaton_free(a_or_b);
}

Test(matching, ab_or_cstar)
{
    Automaton *ab_or_cstar =
        automaton_from_daut(TEST_PATH "automaton/ab+cstar.daut", 9);
    Match *match;

    match = match_nfa(ab_or_cstar, "c");
    Match expected = {
        .string = "c", .start = 0, .length = 1, .nb_groups = 0, .groups = NULL
    };
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "ca");
    expected.string = "ca";
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "ab");
    expected.string = "ab";
    expected.length = 2;
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "ccccccccccccc");
    expected.string = "ccccccccccccc";
    expected.length = 13;
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "");
    expected.string = "";
    expected.length = 0;
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "accccccccccccca");
    expected.string = "accccccccccccca";
    assert_match_eq(&expected, match);
    free(match);

    match = match_nfa(ab_or_cstar, "adibou");
    expected.string = "adibou";
    assert_match_eq(&expected, match);
    free(match);

    automaton_free(ab_or_cstar);
}

/*
Test(matching, email)
{
    Array *email_tokens = tokenize("\\w+(\\.\\w+)?@\\w+(\\.[a-z]{2,})+");
    BinTree *b = parse_symbols(email_tokens);
    Automaton *email = thompson(b);

    cr_assert(match_nfa(email, "rostan.tabet@gmail.com"));
    cr_assert(match_nfa(email, "rostan.tabet@gmail.fr.edu"));
    cr_assert(!match_nfa(email, ".tabet@gmail.com"));
    cr_assert(!match_nfa(email, "rostan.tabet@gmail.c"));
    cr_assert(!match_nfa(email, "rostan.tabet.gmail.com"));
    cr_assert(!match_nfa(email, ""));

    automaton_free(email);
    array_free(email_tokens);
    bintree_free(b);
}
*/

static void free_matches_array(Array *arr)
{
    arr_foreach(Match *, match, arr) free_match(match);

    array_free(arr);
}

Test(substring, abba)
{
    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut", 6);
    Array *matches;
    Match *match;

    matches = search_nfa(abba, "abba");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    Match expected = { .string = "abba",
                       .start = 0,
                       .length = 4,
                       .nb_groups = 0,
                       .groups = NULL };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_nfa(abba, "aaabbaa");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "aaabbaa";
    expected.start = 2;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_nfa(abba, "abbaabba");
    cr_assert_eq(matches->size, 2, "expected 2, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "abbaabba";
    expected.start = 0;
    assert_match_eq(&expected, match);
    match = *(Match **)array_get(matches, 1);
    expected.start = 4;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_nfa(abba, "abb");
    cr_assert_eq(matches->size, 0, "expected 0, got %zu", matches->size);
    free_matches_array(matches);

    automaton_free(abba);
}

Test(substring, abstara)
{
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/abstara.daut", 5);
    Array *matches;
    Match *match;

    matches = search_nfa(aut, "abba");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    Match expected = { .string = "abba",
                       .start = 0,
                       .length = 4,
                       .nb_groups = 0,
                       .groups = NULL };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_nfa(aut, "abbbbbbaaa");
    cr_assert_eq(matches->size, 2, "expected 2, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "abbbbbbaaa";
    expected.start = 0;
    expected.length = 8;
    assert_match_eq(&expected, match);
    match = *(Match **)array_get(matches, 1);
    expected.string = "abbbbbbaaa";
    expected.start = 8;
    expected.length = 2;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_nfa(aut, "aasaaabbbba4abavabaaajaa");
    //                         012345678901234567890123
    cr_assert_eq(matches->size, 7, "expected 7, got %zu", matches->size);
    expected.string = "aasaaabbbba4abavabaaajaa";

    match = *(Match **)array_get(matches, 0);
    expected.start = 0;
    expected.length = 2;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 1);
    expected.start = 3;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 2);
    expected.start = 5;
    expected.length = 6;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 3);
    expected.start = 12;
    expected.length = 3;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 4);
    expected.start = 16;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 5);
    expected.start = 19;
    expected.length = 2;
    assert_match_eq(&expected, match);

    match = *(Match **)array_get(matches, 6);
    expected.start = 22;
    assert_match_eq(&expected, match);

    free_matches_array(matches);

    matches = search_nfa(aut, "abbcba");
    cr_assert_eq(matches->size, 0, "expected 0, got %zu", matches->size);
    free_matches_array(matches);

    automaton_free(aut);
}

Test(substring_dfa, abcdabd)
{
    Automaton *tmp = automaton_from_daut(TEST_PATH "automaton/abcdabd.daut", 7);
    Automaton *aut = build_search_dfa(tmp);
    automaton_free(tmp);
    Array *matches;
    Match *match;

    matches = search_dfa(aut, "abcdabd");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    Match expected = {
        .string = "abcdabd",
        .start = 0,
        .length = 7,
        .nb_groups = 0,
        .groups = NULL,
    };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "eaabcdabd");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "eaabcdabd";
    expected.start = 2;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "ababcdabd");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "ababcdabd";
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "abcabcdabdd");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "abcabcdabdd";
    expected.start = 3;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "abcdabcdab");
    cr_assert_eq(matches->size, 0, "expected 0, got %zu", matches->size);
    free_matches_array(matches);

    automaton_free(aut);
}

Test(substring_dfa, astar_maybe_bc)
{
    Automaton *tmp = automaton_from_daut(TEST_PATH "automaton/a*(bc)?.daut", 7);
    Automaton *aut = build_search_dfa(tmp);
    automaton_free(tmp);
    Array *matches;
    Match *match;

    matches = search_dfa(aut, "abc");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    Match expected = {
        .string = "abc",
        .start = 0,
        .length = 3,
        .nb_groups = 0,
        .groups = NULL,
    };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "bbabc");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "bbabc";
    expected.start = 2;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "aabcaaaabc");
    cr_assert_eq(matches->size, 2, "expected 2, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "aabcaaaabc";
    expected.start = 0;
    expected.length = 4;
    assert_match_eq(&expected, match);
    match = *(Match **)array_get(matches, 1);
    expected.start = 4;
    expected.length = 6;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "baaabd");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "baaabd";
    expected.start = 1;
    expected.length = 3;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    automaton_free(aut);
}

Test(substring_dfa, astar)
{
    Automaton *tmp = automaton_from_daut(TEST_PATH "automaton/a*.daut", 1);
    Automaton *aut = build_search_dfa(tmp);
    automaton_free(tmp);
    Array *matches;
    Match *match;

    matches = search_dfa(aut, "a");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    Match expected = {
        .string = "a",
        .start = 0,
        .length = 1,
        .nb_groups = 0,
        .groups = NULL,
    };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    matches = search_dfa(aut, "baaab");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    expected.string = "baaab";
    expected.start = 1;
    expected.length = 3;
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    automaton_free(aut);
}

Test(substring_dfa, a_b_or_c_a_groups)
{
    Automaton *tmp = automaton_from_daut(
        TEST_PATH "automaton/determine_daut/((a(b|c))a)_determined.daut", 5);
    // Automaton *aut = build_search_dfa(tmp);
    // automaton_free(tmp);
    Automaton *aut = tmp;
    Array *matches;
    Match *match;

    puts("before");
    matches = search_dfa(aut, "aba");
    puts("after");
    cr_assert_eq(matches->size, 1, "expected 1, got %zu", matches->size);
    match = *(Match **)array_get(matches, 0);
    char *groups[] = { "aba", "ab", "b" };
    Match expected = {
        .string = "aba",
        .start = 0,
        .length = 3,
        .nb_groups = 3,
        .groups = groups,
    };
    assert_match_eq(&expected, match);
    free_matches_array(matches);

    automaton_free(aut);
}

// --------------------

Test(replace, abstara)
{
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/abstara.daut", 5);
    char *string;

    string = replace_nfa(aut, "abbbbaabba", "");
    cr_assert_str_eq(string, "", "expected empty string, got '%s'", string);
    free(string);

    string = replace_nfa(aut, "aasabbbba4abavabbbajaa", "");
    cr_assert_str_eq(string, "s4vj", "expected 's4vj', got '%s'", string);
    free(string);

    automaton_free(aut);
}

/*
Test(replace, comma)
{
    Array *tokens = tokenize(", *");
    BinTree *b = parse_symbols(tokens);
    Automaton *aut = thompson(b);
    char *string;

    string = replace_nfa(aut, "1, 2,3,     4,  5", ", ");
    cr_assert_str_eq(string, "1, 2, 3, 4, 5",
                     "expected '1, 2, 3, 4, 5', got '%s'", string);
    free(string);

    string = replace_nfa(aut, "", "azerty");
    cr_assert_str_eq(string, "", "expected empty string, got %s", string);
    free(string);

    automaton_free(aut);
}
*/