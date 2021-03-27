#include <criterion/criterion.h>

#include "automaton/thompson.h"
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

Test(thompson, automaton_a)
{
    Symbol symb = { .type = LETTER, .value = (SymbolValue){ .letter = 'a' } };
    BinTree *tree = BinTree(Symbol, &symb, .left = NULL, .right = NULL);
    Automaton *aut = thompson(tree);
    cr_assert_eq(aut->size, 2);
    automaton_free(aut);
    bintree_free(tree);
}


Test(thompson, automaton_a_b)
{
    Symbol concat = { .type = OPERATOR,
                      .value = (SymbolValue){ .operator= CONCATENATION } };
    Symbol a = { .type = LETTER, .value = (SymbolValue){ .letter = 'a' } };
    Symbol b = { .type = LETTER, .value = (SymbolValue){ .letter = 'b' } };
    BinTree *tree_a = BinTree(Symbol, &a, .right = NULL, .left = NULL);
    BinTree *tree_b = BinTree(Symbol, &b, .right = NULL, .left = NULL);
    BinTree *tree = BinTree(Symbol, &concat, .right = tree_b, .left = tree_a);
    Automaton *aut = thompson(tree);
    //print_automaton(aut);
    cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    bintree_free(tree);
}

Test(thompson, automaton_a_or_b)
{
    Symbol concat = { .type = OPERATOR,
                      .value = (SymbolValue){ .operator= UNION} };
    Symbol a = { .type = LETTER, .value = (SymbolValue){ .letter = 'a' } };
    Symbol b = { .type = LETTER, .value = (SymbolValue){ .letter = 'b' } };
    BinTree *tree_a = BinTree(Symbol, &a, .right = NULL, .left = NULL);
    BinTree *tree_b = BinTree(Symbol, &b, .right = NULL, .left = NULL);
    BinTree *tree = BinTree(Symbol, &concat, .right = tree_b, .left = tree_a);
    Automaton *aut = thompson(tree);
    //print_automaton(aut);
     //cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    bintree_free(tree);
}

/*Test(thompson, automaton_a_or_b_c)
{

    Array *arr = tokenize("(aabb|bbaa)c");
    BinTree *b = parse_symbols(arr);
    tree_to_dot(b);
    array_free(arr);
    Automaton *aut = thompson(b);
    automaton_to_dot(aut);
     //cr_assert_eq(aut->size, 4);
    automaton_free(aut);
    bintree_free(b);
}
*/
