#pragma once
#include "datatypes/array.h"

/**
 * Type used to represent letters in regular expressions.
 * Equivalent to char for now, meaning that regular expressions only
 * support ASCII.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef char Letter;

/**
 * Used to distinguish between the two types of tokens: literal and punctuation.
 * Punctuation includes parenthesis, operators, etc.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum TokenType
{
    LITERAL;
    PUNCTUATION;
} TokenType;

/**
 * Representation of tokens returned by the lexer.
 * Either a literal or punctuation.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct Token
{
    TokenType type;
    Letter value;
} Token;

/**
 * Tokenize a regex string.
 * @author Rostan Tabet
 * @date 04/03/2021
 * @return The array of tokens.
 * @warning The array needs to be freed.
 */
Array *tokenize(char *string);
