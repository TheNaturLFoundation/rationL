#pragma once

/**
 * Type used to represent letters in regular expressions.
 * Equivalent to char for now, meaning that regular expressions only
 * support ASCII.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef char Letter;


/**
 * Used to distinguish between the two types of symbols contained in
 * regular expressions: letters and operators.
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum SymbolType
{
    OPERATOR;
    LETTER;
} SymbolType;

/**
 * Representation of the three regular operators:<br/>
 * - Union<br/>
 * - Concatenation<br/>
 * - Kleene star<br/>
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef enum Operator
{
    KLEENE_STAR;
    CONCATENATION;
    UNION;
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
 * It can be either a letter of the alphabet or
 * one of the three regular operators (union, concatenation, Kleene star)
 * @author Rostan Tabet
 * @date 04/03/2021
 */
typedef struct Symbol
{
    SymbolType type;
    SymbolValue value;
} Symbol;