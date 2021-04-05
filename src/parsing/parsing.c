#include "parsing/parsing.h"

#include <stdio.h>

#include "datatypes/array.h"
#include "datatypes/bin_tree.h"
#include "parsing/lexer.h"

BinTree *parse_paranthesis(BinTree *left, Array *arr, size_t *pos);
BinTree *parse_sub(Array *arr, size_t *pos);
BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos);
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos);

/**
 * Turn a token into a symbol
 * @author Antoine Sicard
 * @date 13/03/2021
 * @return The created symbol
 */
Symbol token_to_symbol(Token *token)
{
    Symbol symbol;
    SymbolType symbolType;
    SymbolValue value;

    if (token->type == LITERAL)
    {
        symbolType = LETTER;
        symbol.type = symbolType;
        value.letter = token->value;
    }
    else
    {
        symbolType = OPERATOR;
        symbol.type = symbolType;
        switch (token->value)
        {
            case '.':
                value.operator= CONCATENATION;
                break;
            case '|':
                value.operator= UNION;
                break;
            case '*':
                value.operator= KLEENE_STAR;
                break;
            case '+':
                value.operator= EXISTS;
                break;
            case '?':
                value.operator= MAYBE;
                break;
        }
    }
    symbol.value = value;
    return symbol;
}

Symbol array_element_to_symbol(Array *arr, size_t pos)
{
    Token *token = array_get(arr, pos);
    Symbol symbol = token_to_symbol(token);
    return symbol;
}

int is_binary(Token *token)
{
    switch (token->value)
    {
        case '.':
        case '|':
            return 1;
        default:
            return 0;
    }
}

int is_unary(Token *token)
{
    switch (token->value)
    {
        case '*':
        case '+':
        case '?':
            return 1;
        default:
            return 0;
    }
}

//   * + and ?    (KLEENE_STAR EXISTS and MAYBE)
BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    // Unused variable ?
    //Token *prev_token = array_get(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
    b->left = left;

    *pos += 1;
    return b;
}

//   . and |    (CONCATENATION and UNION)
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    Token *prev_token = array_get(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
    b->left = left;

    // There is always something after a binary operator
    // No need to check the validity of the position
    *pos += 1;
    Token *current_token = array_get(arr, *pos);

    // After a binary operator, there is either a letter or an opening paranthesis
    if (current_token->type == LITERAL)
    {
        // Handle priorities/closing paranthesis
        if (*pos + 1 < arr->size)
        {
            Token *next_token = array_get(arr, *pos+1);
            // Binary priorities

            if ( next_token->value == '.' &&
                 (prev_token->value == '|' || prev_token->value == '.'))
            {
                b->right = parse_sub(arr, pos);
                /*
                // Check for unary operator
                if (*pos < arr->size)
                {
                    Token *tok = array_get(arr, *pos);
                    if (is_unary(tok))
                        return parse_unary(b, arr, pos);
                }
                */
                return b;
            }
            if (is_binary(next_token))
            {
                Symbol s = array_element_to_symbol(arr, *pos);
                b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
                *pos += 1;
                return parse_binary(b, arr, pos);
            }
            // Unary priorities
            if (is_unary(next_token))
            {
                Symbol s = array_element_to_symbol(arr, *pos);
                BinTree *tmp = BinTree(Symbol, &s, .left = NULL, .right = NULL);
                *pos += 1;
                b->right = parse_unary(tmp, arr, pos);
                return b;
            }
            if (next_token->value == ')')
            {
                Symbol s = array_element_to_symbol(arr, *pos);
                b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
                *pos += 1;
                return b;
            }
        }  // LCOV_EXCL_LINE
        // No particular priority
        Symbol s = array_element_to_symbol(arr, *pos);
        b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
        *pos += 1;
        return b;
    }
    else // Opening paranthesis
    {
        b->right = parse_sub(arr, pos);
        return b;
    }

    return NULL;
}


BinTree *parse_sub(Array *arr, size_t *pos)
{
    Token *token = array_get(arr, *pos);
    // token is an operator
    if(token->value == '(')
    {
        *pos += 1;
        BinTree *tree = parse_sub(arr, pos);
        if (*pos < arr->size)
        {
            token = array_get(arr, *pos);
            if (token->value == ')')
                *pos += 1;
            if (*pos < arr->size)
            {
                token = array_get(arr, *pos);
                if (is_binary(token))
                {
                    return parse_binary(tree, arr, pos);
                }
                if (is_unary(token))
                {
                    return parse_unary(tree, arr, pos);
                }
            }
        }
        return tree;

    }

    Symbol symbol = array_element_to_symbol(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
    *pos += 1;

    // Check the validity of the position
    if (*pos >= arr->size)
        return b;

    token = array_get(arr, *pos);
    if (token->value == ')')
    {
        *pos += 1;
        /*
        if (*pos < arr->size)
        {
            if (is_binary(token))
                return parse_binary(b, arr, pos);
            if (is_unary(token))
                return parse_unary(b, arr, pos);
        }
        */
        // Check binary unary closing paranthesis
        return b;
    }
    if (is_binary(token))
        return parse_binary(b, arr, pos);
    //if (is_unary(token))
        return parse_unary(b, arr, pos);
    //return NULL;
}

BinTree *parse_symbols(Array *arr)
{
    if (arr == NULL)
        return NULL;

    size_t pos = 0;
    // Unused variable?
    //size_t size = arr->size;
    BinTree *b = parse_sub(arr, &pos);

    while (pos < arr->size)
    {
        Token *token = array_get(arr, pos);
        if (is_binary(token))
            b = parse_binary(b, arr, &pos);
        else
        {
            if (is_unary(token))
                b = parse_unary(b, arr, &pos);
            else
                pos += 1;
        }
    }
    return b;
}
