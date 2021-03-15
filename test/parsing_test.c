#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "parsing/parsing.h"
#include <stdio.h>

/*  TESTED
    - NULL
    - "a"

    - "ab"
    - "abc"
    - "abcd"

    - "a|b"
    - "a|b|c"

    - "ab|c"
    - "a|bc"
    - "a|bc|d"
    - "ab|cd"

    - "a\\.cd" (user input : "a\.cd")

    - "a*"
    - "ab*"
    - "a|b*"
    - "a*b"
*/


Test(parse_symbols, null_arg)
{
    Array *arr = NULL;
    BinTree *b = parse_symbols(arr);
    cr_assert_eq(b, NULL);
}


Test(parse_symbol, one_element)
{
    Array *arr = tokenize("a");
    BinTree *b = parse_symbols(arr);
    array_free(arr);

    //   EXPECTED :
    //   a

    Symbol symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');

    void *b_left = b->left;
    cr_assert_eq(b_left, NULL);

    void *b_right = b->right;
    cr_assert_eq(b_right, NULL);

    bintree_free(b);
}


Test(parse_symbols, ab)
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


Test(parse_symbols, abc)
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


Test(parse_symbols, a_or_b)
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


Test(parse_symbols, a_or_b_or_c)
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


Test(parse_symbols, ab_or_c)
{
    Array *arr = tokenize("ab|c");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //     |
    //    / \
    //   .   c
    //  /\
    // a  b


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


Test(parse_symbols, a_or_bc)
{
    Array *arr = tokenize("a|bc");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //   |
    //  / \
    // a   .
    //     /\
    //    b  c


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


Test(parse_symbols, a_or_bc_or_d)
{
    Array *arr = tokenize("a|bc|d");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //     |
    //    / \
    //   |   d
    //  / \
    // a   .
    //    / \
    //   b   c

    // data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);


    // data in b.left == UNION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.left.left == UNION ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.left.right == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.right.left == 'b' ?
    symbol = (*(Symbol *)(b->left->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left->left, NULL);
    cr_assert_eq(b->left->right->left->right, NULL);

    //data in b.left.right.right == 'c'
    symbol = (*(Symbol *)(b->left->right->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->left->right->right->left, NULL);
    cr_assert_eq(b->left->right->right->right, NULL);

    //data in b.right == 'd'
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'd');
    cr_assert_eq(b->right->left, NULL);
    cr_assert_eq(b->right->right, NULL);
    bintree_free(b);
}


Test(parse_symbols, ab_or_cd)
{
    Array *arr = tokenize("ab|cd");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //     |
    //    / \
    //   .   .
    //  /\   /\
    // a  b c  d


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


Test(parse_symbols, a_dot_bc)
{
    Array *arr = tokenize("a\\.bc");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //         .
    //        / \
    //       .   c
    //      / \
    //     .   b
    //    / \
    //   a   .

    // data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    // data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left->left, NULL);
    cr_assert_eq(b->left->left->left->right, NULL);

    //data in b.left.left.right == '.' ?
    symbol = (*(Symbol *)(b->left->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '.');
    cr_assert_eq(b->left->left->right->left, NULL);
    cr_assert_eq(b->left->left->right->right, NULL);

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


Test(parse_symbol, a_star)
{
    Array *arr = tokenize("a*");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //   *
    //  /
    // a

    //data in b == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right, NULL);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    bintree_free(b);
}


Test(parse_symbol, ab_star)
{
    Array *arr = tokenize("ab*");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //    .
    //   / \
    //  a   *
    //     /
    //    b

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

    //data in b.right == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right->right, NULL);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

Test(parse_symbol, a_or_b_star)
{
    Array *arr = tokenize("a|b*");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //    |
    //   / \
    //  a   *
    //     /
    //    b

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

    //data in b.right == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

Test(parse_symbol, a_star_b)
{
    Array *arr = tokenize("a*b");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //     .
    //    / \
    //   *   b
    //  /
    // a

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
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

Test(parse_symbol, abc_star)
{
    Array *arr = tokenize("abc*");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //      .
    //     / \
    //    .   *
    //   / \  /
    //  a   b c

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, CONCATENATION);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.leftLeft == 'b' ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left, NULL);
    cr_assert_eq(b->left->right->right, NULL);

    //data in b.right == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right->right, NULL);

    //data in b.right.left == 'c' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}


Test(parse_symbol, a_exists)
{
    Array *arr = tokenize("a+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //   +
    //  /
    // a

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

Test(parse_symbol, a_or_b_exists)
{
    Array *arr = tokenize("a|b+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //    |
    //   / \
    //  a   +
    //     /
    //    b

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

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

Test(parse_symbol, a_maybe)
{
    Array *arr = tokenize("a?");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //   ?
    //  /
    // a

    //data in b == MAYBE ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, MAYBE);
    cr_assert_eq(b->right, NULL);

    //data in b.left == 'a' ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left, NULL);
    cr_assert_eq(b->left->right, NULL);

    bintree_free(b);
}

Test(parse_symbol, a_or_b_maybe)
{
    Array *arr = tokenize("a|b?");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //    |
    //   / \
    //  a   ?
    //     /
    //    b

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

    //data in b.right == MAYBE ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, MAYBE);

    //data in b.right.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left, NULL);
    cr_assert_eq(b->right->left->right, NULL);

    bintree_free(b);
}

/*
Test(parse_symbol, a_left_c_or_d_right)
{
    Array *arr = tokenize("a(c|d)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;


    for (int i = 0; i<7; i++)
        {
            Token token = *(Token *)array_get(arr, i);
            printf("\n%i : %i", i, token.type);
            printf("\n%i : %c\n\n", i, token.value);
        }

    array_free(arr);

    //   EXPECTED :
    //      .
    //     / \
    //    .   |
    //   / \ / \
    //  a  b c  d


    //data in b == UNION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    bintree_free(b);
}
*/
