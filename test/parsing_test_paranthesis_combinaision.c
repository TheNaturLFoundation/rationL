#include <criterion/criterion.h>
#include "parsing/parsing.h"

/*

    - "(ab)*"
    - "(a|b)*"
    - "(a*b)*"
    - "a(b|c)*"
    - "a((b|c)*)"
    - "a?(bc)*"
*/

Test(parse_paranthesis_combinaison, left_ab_right_star)
{
    Array *arr = tokenize("(ab)*");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      *
    //     /
    //    .
    //   / \
    //  a   b

    //data in b == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right, NULL);

    //data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);
    bintree_free(b);
}


Test(parse_paranthesis_combinaison, left_a_or_b_right_star)
{
    Array *arr = tokenize("(a|b)*");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //      *
    //     /
    //    |
    //   / \
    //  a   b

    //data in b == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right, NULL);

    //data in b.left == UNION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, UNION);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);
    bintree_free(b);
}


Test(parse_paranthesis_combinaison, left_a_star_b_right_star)
{
    Array *arr = tokenize("(a*b)*");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //       *
    //      /
    //     .
    //    / \
    //   *   b
    //  /
    // a

    //data in b == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right, NULL);

    //data in b.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left.left == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.left.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left->left, NULL);
    cr_assert_eq(b->left->left->left->right, NULL);

    //data in b.left.right == 'b' ?
    symbol = (*(Symbol *)(b->left->right->data));
    cr_assert_eq(symbol.type, CONCATENATION);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->left->right->left, NULL);
    cr_assert_eq(b->left->right->right, NULL);

    bintree_free(b);
}

Test(parse_paranthesis_combinaison, a_left_b_or_c_right_star)
{
    Array *arr = tokenize("a(b|c)*");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //     .
    //    / \
    //   a   *
    //      /
    //      |
    //     / \
    //    b   c

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

    //data in b.right.left == UNION ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, UNION);

    //data in b.right.left.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left->left, NULL);
    cr_assert_eq(b->right->left->left->right, NULL);

    //data in b.right.left.right == 'c' ?
    symbol = (*(Symbol *)(b->right->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->right->left, NULL);
    cr_assert_eq(b->right->left->right->right, NULL);

    bintree_free(b);
}

Test(parse_paranthesis_combinaison, a_left_left_b_or_c_right_star_right)
{
    Array *arr = tokenize("a((b|c)*)");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //     .
    //    / \
    //   a   *
    //      /
    //      |
    //     / \
    //    b   c

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

    //data in b.right.left == UNION ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, UNION);

    //data in b.right.left.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left->left, NULL);
    cr_assert_eq(b->right->left->left->right, NULL);

    //data in b.right.left.right == 'c' ?
    symbol = (*(Symbol *)(b->right->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->right->left, NULL);
    cr_assert_eq(b->right->left->right->right, NULL);

    bintree_free(b);
}

Test(parse_paranthesis_combinaison, a_maybe_left_bc_right_star)
{
    Array *arr = tokenize("a?(bc)*");
    BinTree *b = parse_symbols(arr);
    Symbol symbol;

    array_free(arr);

    //   EXPECTED :
    //     .
    //    / \
    //   ?   *
    //  /   /
    // a    .
    //     / \
    //    b   c

    //data in b == CONCATENATION ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, CONCATENATION);

    //data in b.left == MAYBE ?
    symbol = (*(Symbol *)(b->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, MAYBE);
    cr_assert_eq(b->left->right, NULL);

    //data in b.left.left == 'a' ?
    symbol = (*(Symbol *)(b->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'a');
    cr_assert_eq(b->left->left->left, NULL);
    cr_assert_eq(b->left->left->right, NULL);

    //data in b.right == KLEENE_STAR ?
    symbol = (*(Symbol *)(b->right->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.operator, KLEENE_STAR);
    cr_assert_eq(b->right->right, NULL);

    //data in b.right.left == CONCATENATION ?
    symbol = (*(Symbol *)(b->right->left->data));
    cr_assert_eq(symbol.type, OPERATOR);
    cr_assert_eq(symbol.value.letter, CONCATENATION);

    //data in b.right.left.left == 'b' ?
    symbol = (*(Symbol *)(b->right->left->left->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'b');
    cr_assert_eq(b->right->left->left->left, NULL);
    cr_assert_eq(b->right->left->left->right, NULL);

    //data in b.right.left.right == 'c' ?
    symbol = (*(Symbol *)(b->right->left->right->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, 'c');
    cr_assert_eq(b->right->left->right->left, NULL);
    cr_assert_eq(b->right->left->right->right, NULL);

    bintree_free(b);
}
