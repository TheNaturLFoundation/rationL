#include <criterion/criterion.h>
#include "parsing/parsing.h"

/*

    - "\\." (user input : "a\.cd")
    - "\\|" (user input : "a\.cd")
    - "\\*" (user input : "a\.cd")
    - "\\+" (user input : "a\.cd")
    - "\\?" (user input : "a\.cd")
*/

Test(parse_escaped_character, a_dot_b)
{
    Array *arr = tokenize("\\.");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //       .

    // data in b == '.' ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '.');

    bintree_free(b);
}

Test(parse_escaped_character, a_escaped_union_b)
{
    Array *arr = tokenize("\\|");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //       |

    // data in b == '|' ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '|');

    bintree_free(b);
}

Test(parse_escaped_character, a_escaped_star_b)
{
    Array *arr = tokenize("\\*");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //       *

    // data in b == '*' ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '*');

    bintree_free(b);
}

Test(parse_escaped_character, a_escaped_exists_b)
{
    Array *arr = tokenize("\\+");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //       +

    // data in b == '+' ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '+');

    bintree_free(b);
}

Test(parse_escaped_character, a_escaped_maybe_b)
{
    Array *arr = tokenize("\\?");
    BinTree *b = parse_symbols(arr);
    array_free(arr);
    Symbol symbol;

    //   EXPECTED :
    //       ?

    // data in b == '?' ?
    symbol = (*(Symbol *)(b->data));
    cr_assert_eq(symbol.type, LETTER);
    cr_assert_eq(symbol.value.letter, '?');

    bintree_free(b);
}
