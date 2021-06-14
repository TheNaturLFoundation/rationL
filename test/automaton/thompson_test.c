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

Test(thompson, automaton_a, .disabled = 1)
{
    // Disabled because for a simple string
    // a DFA is directly constructed
    Array *arr = tokenize("a");
    BinTree *b = parse_symbols(arr);
    Automaton *aut = thompson(b);
    //automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, automaton_a_b, .disabled = 1)
{
    // Disabled because for a simple string
    // a DFA is directly constructed
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
    State * s0 = *(State **)array_get(aut->states, 0);
    State * s1 = *(State **)array_get(aut->states, 1);
    Map * set;

    cr_assert_eq(get_leaving_group(aut, s0, s1, 'a', 0), NULL);
    set = get_entering_groups(aut, NULL, s0, 0, 1), NULL;

    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

    set = get_leaving_group(aut, s1, NULL, 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

    cr_assert_eq(get_entering_groups(aut, NULL, s1, 0, 0), NULL);

    //automaton_to_dot(aut);
    
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, simple_a_b)
{
    Array *arr = tokenize("(a)(b)");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *aut = thompson(b);
    State * s[4];
    for(int i = 0; i < 4; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

    //automaton_to_dot(aut);
    
    cr_assert_eq(get_entering_groups(aut, NULL, s[0], 0, 0), NULL);

    set = get_leaving_group(aut, s[3], s[0], 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

    set = get_entering_groups(aut, s[3], s[0], 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, verifying_not_gettting_out_of_0)
{
    Array *arr = tokenize("q(a(bc))");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *aut = thompson(b);

    State * s[4];
    for(int i = 0; i < 4; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

    //automaton_to_dot(aut);

    cr_assert_eq(get_leaving_group(aut, s[3], 
        s[0], 0, 1), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, full_union)
{
    Array *arr = tokenize("(a|b)");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);

    State * s[6];
    for(int i = 0; i < 6; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

   // automaton_to_dot(aut);

    cr_assert_eq(aut->leaving_transitions->size, 1);
    cr_assert_eq(aut->entering_transitions->size, 1,
        "Expected 1 go %lu", aut->entering_transitions->size);

    cr_assert_neq(get_leaving_group(aut, s[5], NULL, 0, 1), NULL);
    cr_assert_neq(get_entering_groups(aut, NULL, s[4], 0, 1), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, union_isolate_right)
{
    Array *arr = tokenize("(a)|b");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);

    State * s[6];
    for(int i = 0; i < 6; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

//    automaton_to_dot(aut);

    cr_assert_eq(aut->leaving_transitions->size, 1);
    cr_assert_eq(aut->entering_transitions->size, 1,
        "Expected 1 go %lu", aut->entering_transitions->size);

    cr_assert_neq(get_entering_groups(aut, s[4], s[0], 0, 1), NULL);
    cr_assert_neq(get_leaving_group(aut, s[1], s[5], 0, 1), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, union_isolate_left)
{
    Array *arr = tokenize("a|(b)");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);

    State * s[6];
    for(int i = 0; i < 6; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

//    automaton_to_dot(aut);

    cr_assert_eq(aut->leaving_transitions->size, 1);
    cr_assert_eq(aut->entering_transitions->size, 1,
        "Expected 1 go %lu", aut->entering_transitions->size);

    cr_assert_neq(get_entering_groups(aut, s[4], s[2], 0, 1), NULL);
    cr_assert_neq(get_leaving_group(aut, s[3], s[5], 0, 1), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, flex_1)
{
    Array *arr = tokenize("(ab)|(b(de))");
    
    //Array *arr = tokenize("abc");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *aut = thompson(b);

    State * s[12];
    for(int i = 0; i < 12; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

    //automaton_to_dot(aut);

    cr_assert_eq(aut->entering_transitions->size, 3);
    cr_assert_eq(aut->leaving_transitions->size, 2);
    
    //entering first...
    size_t grp;

    set = get_entering_groups(aut, s[10], s[2], 0, 1);
    grp = 1;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    set = get_entering_groups(aut, s[10], s[8], 0, 1);
    grp = 2;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);
    
    set = get_entering_groups(aut, s[9], s[6], 0, 1);
    grp = 3;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    //go for leaving

    set = get_leaving_group(aut, s[1], s[11], 0, 1);
    cr_assert_neq(set, NULL);
    grp = 1;
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    set = get_leaving_group(aut, s[5], s[11], 0, 1);
    cr_assert_neq(set, NULL);
    grp = 2;
    cr_assert_eq(set->size, 2);
    cr_assert_neq(map_get(set, &grp), NULL);
    grp = 3;
    cr_assert_neq(map_get(set, &grp), NULL); 

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, testtttt)
{
    Array *arr = tokenize("(a)*");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);

    State * s[4];
    for(int i = 0; i < 4; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;

    cr_assert_eq(aut->leaving_transitions->size, 1);
    cr_assert_eq(aut->entering_transitions->size, 1);

    set = get_leaving_group(aut, s[3], NULL, 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

    set = get_entering_groups(aut, NULL, s[2], 0, 1);
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);

//    automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, keepsinternals)
{
    Array *arr = tokenize("(a(bc)d)*");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *aut = thompson(b);
    
    //automaton_to_dot(aut);

    State * s[10];
    for(int i = 0; i < 10; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;
    size_t grp;
    cr_assert_eq(aut->leaving_transitions->size, 2);
    cr_assert_eq(aut->entering_transitions->size, 2);

    //let's goooooooo
    set = get_entering_groups(aut, s[7], s[4], 0, 1);
    grp = 2;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    set = get_entering_groups(aut, NULL, s[8], 0, 1);
    grp = 1;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    //lets go for leaaaaaving
    set = get_leaving_group(aut, s[3], s[0], 0, 1);
    grp = 2;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    set = get_leaving_group(aut, s[9], NULL, 0, 1);
    grp = 1;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, testttt)
{
    Array *arr = tokenize("b(ad)+");
    BinTree *b = parse_symbols(arr);
    //bin_tree_to_dot(b, stdout);
    Automaton *aut = thompson(b);
    
    //automaton_to_dot(aut);
    
    State * s[7];
    for(int i = 0; i < 7; i++)
    {
        s[i] = *(State **)array_get(aut->states, i);
    }
    Set * set;
    size_t grp = 1;

    cr_assert_eq(aut->leaving_transitions->size, 1);
    cr_assert_eq(aut->entering_transitions->size, 1);

    set = get_entering_groups(aut, s[6], s[2], 0, 1);
    grp = 1;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    set = get_leaving_group(aut, s[4], NULL, 0, 1);
    grp = 1;
    cr_assert_neq(set, NULL);
    cr_assert_eq(set->size, 1);
    cr_assert_neq(map_get(set, &grp), NULL);

    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}

Test(thompson, I_hate_thompson)
{
    Array *arr = tokenize("a|(c(dd))");
    BinTree *b = parse_symbols(arr);
    //tree_to_dot(b);
    Automaton *aut = thompson(b);
    automaton_to_dot(aut);
    automaton_free(aut);
    array_free(arr);
    bintree_free(b);
}
