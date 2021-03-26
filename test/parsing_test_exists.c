#include <criterion/criterion.h>
#include "parsing/parsing.h"

/*

    - "a+"
    - "ab+"
    - "a|b+"
    - "a+b"
    - "a+|b"
*/

Test(parse_exists, a_exists)
{
    Array *arr = tokenize("a+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
         +
        /
       a
    */

    //data in b == EXISTS ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, EXISTS);
    cr_assert_eq(b->right, NULL);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    bintree_free(b);
}


Test(parse_exists, ab_exists)
{
    Array *arr = tokenize("ab+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
          .
         / \
        a   +
           /
          b
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == EXISTS ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, EXISTS);
    cr_assert_eq(b->right->right, NULL);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

Test(parse_exists, a_or_b_exists)
{
    Array *arr = tokenize("a|b+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
          |
         / \
        a   +
           /
          b
    */

    //data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == EXISTS ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, EXISTS);
    cr_assert_eq(b->right->right, NULL);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

Test(parse_exists, a_exists_b)
{
    Array *arr = tokenize("a+b");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
           .
          / \
         +   b
        /
       a
    */

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == EXISTS ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, EXISTS);
    cr_assert_eq(b->left->right, NULL);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.right == 'b' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}

Test(parse_exists, a_exists_or_b)
{
    Array *arr = tokenize("a+|b");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    /*   EXPECTED :
           |
          / \
         +   b
        /
       a
    */

    //data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.left == EXISTS ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, EXISTS);
    cr_assert_eq(b->left->right, NULL);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.right == 'b' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}
