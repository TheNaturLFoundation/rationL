#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "rationl/automaton.h"
#include "rationl/thompson.h"
#include "rationl/bin_tree.h"
#include "rationl/array.h"
#include "rationl/parsing.h"
#include "rationl/matching.h"

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
