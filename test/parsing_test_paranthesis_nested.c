#include <criterion/criterion.h>
#include "parsing/parsing.h"

/*

    - "a(c|d)"
    - "a|(c|d)"
    - "a((c|d))"
    - "a(((c|d)))"
*/

Test(parse_paranthesis_nested, a_left_c_or_d_right)
{
    Array *arr = tokenize("a(c|d)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      .
    //     / \
    //    a   |
    //       / \
    //       c  d


    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    //data in b.right == UNION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');

    //data in b.right.right == 'd' ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');

    bintree_free(b);
}


Test(parse_paranthesis_nested, a_or_left_c_or_d_right)
{
    Array *arr = tokenize("a|(c|d)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      |
    //     / \
    //    a   |
    //       / \
    //       c  d


    //data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    //data in b.right == UNION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');

    //data in b.right.right == 'd' ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');

    bintree_free(b);
}


Test(parse_paranthesis_nested, a_left_left_c_or_d_right_right)
{
    Array *arr = tokenize("a((c|d))");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      .
    //     / \
    //    a   |
    //       / \
    //       c  d


    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    //data in b.right == UNION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');

    //data in b.right.right == 'd' ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');

    bintree_free(b);
}


Test(parse_paranthesis_nested, a_left_left_left_c_or_d_right_right_right)
{
    Array *arr = tokenize("a(((c|d)))");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      .
    //     / \
    //    a   |
    //       / \
    //       c  d


    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    //data in b.right == UNION ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');

    //data in b.right.right == 'd' ?
    symbol = (*(Symbol *)(b->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');

    bintree_free(b);
}
