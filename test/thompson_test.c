#include <criterion/criterion.h>

#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(thompson, automaton_a)
{
    Array *arr = tokenize("a");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
    cr_assert_eq(aut->size, 2);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_b)
{
    Array *arr = tokenize("ab");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    // print_automaton(aut);
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
