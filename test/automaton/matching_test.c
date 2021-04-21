#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "datatypes/array.h"
#include "parsing/parsing.h"
#include "matching/matching.h"

/*
Test(matching, abba)
{
    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut");
    cr_assert(match_nfa(abba, "abba"));
    cr_assert(!match_nfa(abba, "abb"));
    cr_assert(!match_nfa(abba, "abbaa"));
    cr_assert(!match_nfa(abba, ""));

    automaton_free(abba);
}

Test(matching, a_or_b)
{
    Automaton *a_or_b = automaton_from_daut(TEST_PATH "automaton/a+b.daut");
    cr_assert(match_nfa(a_or_b, "a"));
    cr_assert(match_nfa(a_or_b, "b"));
    cr_assert(!match_nfa(a_or_b, "c"));
    cr_assert(!match_nfa(a_or_b, "ab"));
    cr_assert(!match_nfa(a_or_b, "ba"));
    cr_assert(!match_nfa(a_or_b, ""));

    automaton_free(a_or_b);
}

Test(matching, ab_or_cstar)
{
    Automaton *ab_or_cstar = automaton_from_daut(TEST_PATH "automaton/ab+cstar.daut");
    cr_assert(match_nfa(ab_or_cstar, "c"));
    cr_assert(match_nfa(ab_or_cstar, "ab"));
    cr_assert(match_nfa(ab_or_cstar, ""));
    cr_assert(match_nfa(ab_or_cstar, "ccccccccccccc"));
    cr_assert(!match_nfa(ab_or_cstar, "ccccccccccccca"));
    cr_assert(!match_nfa(ab_or_cstar, "ac"));

    automaton_free(ab_or_cstar);
}

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

static void free_substring_array(Array *arr)
{
    arr_foreach(char *, s, arr)
        free(s);

    array_free(arr);
}

Test(substring, abba)
{
    Array *subs;
    Automaton *abba = automaton_from_daut(TEST_PATH "automaton/abba.daut");

    subs = search_nfa(abba, "abba");
    cr_assert_eq(subs->size, 1, "expected 1, got %zu", subs->size);
    char *s1 = *(char **)array_get(subs, 0);
    cr_assert_str_eq(s1, "abba", "expected 'abba', got '%s'", s1);
    free_substring_array(subs);

    subs = search_nfa(abba, "aaabbaa");
    cr_assert_eq(subs->size, 1, "expected 1, got %zu", subs->size);
    char *s2 = *(char **)array_get(subs, 0);
    cr_assert_str_eq(s2, "abba", "expected 'abba', got '%s'", s2);
    free_substring_array(subs);

    subs = search_nfa(abba, "abbaabba");
    cr_assert_eq(subs->size, 2, "expected 2, got %zu", subs->size);
    arr_foreach(char *, s3, subs)
        cr_assert_str_eq(s3, "abba", "expected 'abba', got '%s'", s3);
    free_substring_array(subs);

    subs = search_nfa(abba, "abb");
    cr_assert_eq(subs->size, 0, "expected 0, got %zu", subs->size);
    free_substring_array(subs);

    automaton_free(abba);
}

Test(substring, abstara)
{
    Array *subs;
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/abstara.daut");

    subs = search_nfa(aut, "abba");
    cr_assert_eq(subs->size, 1, "expected 1, got %zu", subs->size);
    char *s1 = *(char **)array_get(subs, 0);
    cr_assert_str_eq(s1, "abba", "expected 'abba', got '%s'", s1);
    free_substring_array(subs);

    subs = search_nfa(aut, "abbbbbbaaa");
    cr_assert_eq(subs->size, 2, "expected 2, got %zu", subs->size);
    char *s2 = *(char **)array_get(subs, 0);
    cr_assert_str_eq(s2, "abbbbbba", "expected 'abbbbbba', got '%s'", s2);
    s2 = *(char **)array_get(subs, 1);
    cr_assert_str_eq(s2, "aa", "expected 'aa', got '%s'", s2);
    free_substring_array(subs);

    subs = search_nfa(aut, "aasaaabbbba4abavabaaajaa");
    cr_assert_eq(subs->size, 7, "expected 7, got %zu", subs->size);
    char *s3 = *(char **)array_get(subs, 0);
    cr_assert_str_eq(s3, "aa", "expected 'aa', got '%s'", s3);
    s3 = *(char **)array_get(subs, 1);
    cr_assert_str_eq(s3, "aa", "expected 'aa', got '%s'", s3);
    s3 = *(char **)array_get(subs, 2);
    cr_assert_str_eq(s3, "abbbba", "expected 'abbbba', got '%s'", s3);
    s3 = *(char **)array_get(subs, 3);
    cr_assert_str_eq(s3, "aba", "expected 'aba', got '%s'", s3);
    s3 = *(char **)array_get(subs, 4);
    cr_assert_str_eq(s3, "aba", "expected 'aa', got '%s'", s3);
    s3 = *(char **)array_get(subs, 5);
    cr_assert_str_eq(s3, "aa", "expected 'aba', got '%s'", s3);
    s3 = *(char **)array_get(subs, 6);
    cr_assert_str_eq(s3, "aa", "expected 'aa', got '%s'", s3);
    free_substring_array(subs);

    subs = search_nfa(aut, "abbcba");
    cr_assert_eq(subs->size, 0, "expected 0, got %zu", subs->size);
    free_substring_array(subs);

    automaton_free(aut);
}

Test(replace, abstara)
{
    Automaton *aut = automaton_from_daut(TEST_PATH "automaton/abstara.daut");
    char *string;

    string = replace_nfa(aut, "abbbbaabba", "");
    cr_assert_str_eq(string, "", "expected empty string, got '%s'", string);
    free(string);

    string = replace_nfa(aut, "aasabbbba4abavabbbajaa", "");
    cr_assert_str_eq(string, "s4vj", "expected 's4vj', got '%s'", string);
    free(string);

    automaton_free(aut);
}

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
}*/
