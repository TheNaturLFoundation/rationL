#include <criterion/criterion.h>
#include "datatypes/bin_tree.h"
#include "parsing/parsing.h"

/*

    - "ab|c"
    - "a|bc"
    - "a|bc|d"
    - "ab|cd"
    - "ab|cde"
*/


Test(parse_union_and_concatenation, ab_or_c)
{
    Array *arr = tokenize("ab|c");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
           |
          / \
         .   c
        /\
       a  b
    */

    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);


    // data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.left.right == 'b' ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left, NULL);
    cr_assert_eq(b->left->right->right, NULL);

    //data in b.right == 'c' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parse_union_and_concatenation, a_or_bc)
{
    Array *arr = tokenize("a|bc");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
         |
        / \
       a   .
           /\
          b  c
    */

    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);


    // data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    //data in b.right == 'c' ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->right->left, NULL);
    cr_assert_eq(b->right->right->right, NULL);

    bintree_free(b);
}


Test(parse_union_and_concatenation, a_or_bc_or_d)
{
    Array *arr = tokenize("a|bc|d");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED:
           |
          / \
         a   |
            / \
           .   d
          / \
         b   c
    */

    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    // data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == UNION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.right.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.right.left.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left->left, NULL);
    cr_assert_eq(b->right->left->left->right, NULL);

    //data in b.right.left.right == 'c'
    symbol = (*(Symbol *)(b->right->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->right->left, NULL);
    cr_assert_eq(b->right->left->right->right, NULL);

    //data in b.right.right == 'd'
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');
    cr_assert_eq(b->right->right->left, NULL);
    cr_assert_eq(b->right->right->right, NULL);

    bintree_free(b);
}


Test(parse_union_and_concatenation, ab_or_cd)
{
    Array *arr = tokenize("ab|cd");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*
        EXPECTED :
           |
          / \
         .   .
        /\   /\
       a  b c  d
    */

    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    // data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.left.right == 'b' ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left, NULL);
    cr_assert_eq(b->left->right->right, NULL);

    //data in b.right == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.right.left == 'c'
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    //data in b.right.right == 'd'
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');
    cr_assert_eq(b->right->right->left, NULL);
    cr_assert_eq(b->right->right->right, NULL);

    bintree_free(b);
}


Test(parse_union_and_concatenation, aa_or_bbb)
{
    Array *arr = tokenize("ab|cde");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*
        EXPECTED :

                   |
                  / \
                 /   \
                /     \
               .       .
              / \     / \
             a   b   c   .
                        / \
                       d   e
    */

    // data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    // data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    // data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    // data in b.left.right == 'b' ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left, NULL);
    cr_assert_eq(b->left->right->right, NULL);

    // data in b.right == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    // data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    // data in b.right.right == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    // data in b.right.right.left == 'd' ?
    symbol = (*(Symbol *)(b->right->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');
    cr_assert_eq(b->right->right->left->left, NULL);
    cr_assert_eq(b->right->right->left->right, NULL);

    // data in b.right.right.right == 'e' ?
    symbol = (*(Symbol *)(b->right->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'e');
    cr_assert_eq(b->right->right->right->left, NULL);
    cr_assert_eq(b->right->right->right->right, NULL);

    bintree_free(b);
}