#pragma once
#include "lexer.h"
#include "datatypes/array.h"
#include "datatypes/bin_tree.h"

/**
 * Used to distinguish between the two types of symbols contained in
 * regular expressions: letters and operators.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum SymbolType
{
    OPERATOR,
    LETTER
} SymbolType;

/**
 * Representation regular expression operators:<br/>
 * - Concatenation<br/>
 * - Union (|)<br/>
 * - Kleene star (*)<br/>
 * - Exists (+)<br/>
 * - Maybe (?)
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum Operator
{
    KLEENE_STAR,
    CONCATENATION,
    UNION,
    EXISTS,
    MAYBE
} Operator;

/**
 * Representation of a symbol, either a letter or an operator.
 * Do NOT use this type outside the struct Symbol.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef union SymbolValue
{
    Operator operator;
    Letter letter;
} SymbolValue;

/**
 * @struct Symbol
 * @brief Representation of a regex symbol.
 * It can be either a letter of the alphabet or an operator.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct Symbol
{
    SymbolType type;
    SymbolValue value;
} Symbol;

/**
 * Turn a token into a symbol
 * @author Antoine Sicard
 * @date 13/03/2021
 * @return The created symbol
 */
Symbol token_to_symbol(Token token);

BinTree *parse_star_exists_or_maybe(BinTree *left, Array *arr, size_t *pos);
BinTree *parse_union_or_concatenation(BinTree *left, Array *arr, size_t *pos);
BinTree *parse(Array *arr, size_t *pos);


/**
 * Turn an array of tokens into a binary tree of symbols.
 * @author Antoine Sicard
 * @date 04/03/2021
 * @return The binary tree corresponding to the parsed expression.
 */
BinTree *parse_symbols(Array *arr);
