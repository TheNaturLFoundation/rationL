#include <criterion/criterion.h>
#include "parsing/parsing.h"


/*
    - NULL
    - "a"

    - "ab"
    - "abc"

    - "a|b"
    - "a|b|c"

*/

Test(parsing_union_or_concatenation, null_arg)
{
    Array *arr = NULL;
    BinTree *b = parse_symbols(arr);
    cr_assert_eq(b, NULL);
}


Test(parsing_union_or_concatenation, one_element)
{
    Array *arr = tokenize("a");
    BinTree *b = parse_symbols(arr);
    array_free(arr);

    //   EXPECTED :
    //   a


    Symbol symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    cr_assert_eq(b->left, NULL);
    cr_assert_eq(b->right, NULL);

    bintree_free(b);
}


Test(parsing_union_or_concatenation, ab)
{
    Array *arr = tokenize("ab");
    BinTree *b = parse_symbols(arr);
    array_free(arr);

    //   EXPECTED :
    //   .
    //  / \
    // a   b

    // data in b == CONCATENATION ?
    Symbol symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    // data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == 'b' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parsing_union_or_concatenation, abc)
{
    Array *arr = tokenize("abc");
    BinTree *b = parse_symbols(arr);
    array_free(arr);

    //   EXPECTED :
    //      .
    //     / \
    //    .   c
    //   / \
    //  a   b
    //


    // data in b == CONCATENATION ?
    Symbol symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

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


Test(parsing_union_or_concatenation, a_or_b)
{
    Array *arr = tokenize("a|b");
    BinTree *b = parse_symbols(arr);
    array_free(arr);

    //   EXPECTED :
    //   |
    //  / \
    // a   b


    // data in b == UNION ?
    Symbol symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    // data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    //data in b.right == 'b' ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);

    bintree_free(b);
}


Test(parsing_union_or_concatenation, a_or_b_or_c)
{
    Array *arr = tokenize("a|b|c");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //     |
    //    / \
    //   |   c
    //  / \
    // a   b


    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    // data in b.left == UNION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

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
