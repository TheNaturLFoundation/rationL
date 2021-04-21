#include <criterion/criterion.h>

#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"
/*
Test(thompson, null)
{
    Automaton *aut = thompson(NULL);
    cr_assert_eq(aut, NULL);
}

Test(thompson, automaton_a, .disabled = 0)
{
    Array *arr = tokenize("a");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_b)
{
    Array *arr = tokenize("ab");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_or_b)
{
    Array *arr = tokenize("a|b");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    //automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_aabb_or_bbaa_c)
{
    Array *arr = tokenize("(aabb|bbaa)c");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    //automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_star)
{
    Array *arr = tokenize("a*");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    //automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_aa_or_bb_star)
{
    Array *arr = tokenize("(aa|bb)*");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    //automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_aa_or_bb_star_a_star_b_star_c)
{
    Array *arr = tokenize("((aa|b)*c)*|c");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    //automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_b_maybe)
{
    Array *arr = tokenize("(ab)?");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}


Test(thompson, automaton_a_b_exists)
{
    Array *arr = tokenize("(ab)+");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, aa_star)
{
    Array *arr = tokenize("aa*");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, identifier)
{
    Array *arr = tokenize("a{15,}");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}
*/
