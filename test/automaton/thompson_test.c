#include <criterion/criterion.h>

#include "automaton/automaton.h"
#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

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
    Array *arr = tokenize("a(aa|bbaa)");
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
    // automaton_to_dot(aut);
    // cr_assert_eq(aut->size, 4);
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

Test(thompson, group_concat)
{
    Array *arr = tokenize("(b)c");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *automaton = thompson(b);
    //automaton_to_dot(automaton);
    size_t g = 1;
    
    State * s[4];
    for(int i = 0; i < 4; i++)
    {
        s[i] = *(State **)array_get(automaton->states, i);
    }
    Set * set;

    cr_assert_eq(get_entering_groups(automaton, s[2], s[3], 'b', 0), NULL);
    cr_assert_eq(get_entering_groups(automaton, s[3], s[0], 0, 1), NULL);
    cr_assert_eq(get_entering_groups(automaton, s[0], s[1], 'c', 0), NULL);

    set = get_entering_groups(automaton, NULL, s[2], 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &g), NULL);

    //leaving:
    cr_assert_eq(get_leaving_group(automaton, NULL, s[2], 'b', 0), NULL);
    cr_assert_eq(get_leaving_group(automaton, s[2], s[3], 'b', 0), NULL);
    cr_assert_eq(get_leaving_group(automaton, s[0], s[1], 'c', 0), NULL);

    set = get_leaving_group(automaton, s[3], s[0], 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &g), NULL);

    automaton_free(automaton);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, simple)
{
    Array *arr = tokenize("(a)");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    // automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}
