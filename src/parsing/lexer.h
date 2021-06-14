#pragma once
#include "datatypes/array.h"

#define ASCII_FIRST_PRINTABLE 32
#define ASCII_LAST_PRINTABLE 126
// Supported control characters are \t, \n, \v, \f and \r
#define ASCII_FIRST_CONTROL 9
#define ASCII_LAST_CONTROL 13

#define Literal(val) { .type = LITERAL, .value.letter = (Letter) (val) }

#define Punctuation(val) { .type = PUNCTUATION, .value.letter = (Letter) (val) }

/**
 * Type used to represent letters in regular expressions.
 * Equivalent to char for now, meaning that regular expressions only
 * support ASCII.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef unsigned char Letter;

/**
 * Used to distinguish between the two types of tokens: literal and punctuation.
 * Punctuation includes parenthesis, operators, etc.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum TokenType
{
    LITERAL,
    PUNCTUATION,
    CLASS,
} TokenType;


typedef union TokenValue
{
    Letter letter;
    Array* letters;
} TokenValue;

/**
 * Representation of tokens returned by the lexer.
 * Either a literal or punctuation.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct Token
{
    TokenType type;
    TokenValue value;
} Token;

/**
 * Tokenize a regex string.
 * @author Rostan Tabet
 * @date 04/03/2021
 * @return The array of tokens.
 * @warning The array needs to be freed.
 */
Array *tokenize(const char *string);

/**
 * Carefully frees an array of tokens.
 * Also frees the content of the caracter classes.
 * @author Simon Scatton
 * @date 12/06/2021
 * @param tokens The array of tokens to be freed.
 */
void free_tokens(Array *tokens);
