#include <criterion/criterion.h>
#include "parsing/parsing.h"

/*
    - "(a)"
    - "(ab)"
    - "a(b)"
    - "(a)b"
    - "(a)*b"
    - "(a*)"
*/

Test(parse_paranthesis_simple, left_a_right)
{
    Array *arr = tokenize("(a)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           a
    */

    //data in b == a ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    bintree_free(b);
}


Test(parse_paranthesis_simple, left_ab_right)
{
    Array *arr = tokenize("(ab)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           .
          / \
         a   b
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == a ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == b ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parse_paranthesis_simple, a_left_b_right)
{
    Array *arr = tokenize("a(b)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           .
          / \
         a   b
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == a ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == b ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parse_paranthesis_simple, left_a_right_b)
{
    Array *arr = tokenize("(a)b");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           .
          / \
         a   b
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == a ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == b ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parse_paranthesis_simple, left_a_right_star_b)
{
    Array *arr = tokenize("(a)*b");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           .
          / \
         *   b
        /
       a
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == a ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, KLEENE_STAR);
    cr_assert_eq(b->left->right, NULL);

    //data in b.left.left == 'b' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.right == 'a' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parse_paranthesis_simple, left_a_star_right)
{
    Array *arr = tokenize("(a*)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    /*   EXPECTED :
           *
          /
         a
    */

    //data in b == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);

    //data in b.left == a ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->right, NULL);

    bintree_free(b);
}
