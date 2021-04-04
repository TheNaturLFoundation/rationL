#pragma once
#include "array.h"

#define ASCII_FIRST_PRINTABLE 32
#define ASCII_LAST_PRINTABLE 126
// Supported control characters are \t, \n, \v, \f and \r
#define ASCII_FIRST_CONTROL 9
#define ASCII_LAST_CONTROL 13

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
    LITERAL,
    PUNCTUATION
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
Array *tokenize(const char *string);
