#include "parsing.h"

#include <stdio.h>

#include "datatypes/bin_tree.h"
#include "lexer.h"

BinTree *parse_paranthesis(Array *arr, size_t *pos);
BinTree *parse_sub(Array *arr, size_t *pos);
BinTree *parse_unary_operator(BinTree *left, Array *arr, size_t *pos);
BinTree *parse_binary_operator(BinTree *left, Array *arr, size_t *pos);

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
    if (token->type == PUNCTUATION)
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
    return 0;
}

//   * + and ?    (KLEENE_STAR EXISTS and MAYBE)
BinTree *parse_unary_operator(BinTree *left, Array *arr, size_t *pos)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol);

    b->left = left;
    *pos += 1;

    return b;
}

//   . and |    (CONCATENATION and UNION)
BinTree *parse_binary_operator(BinTree *left, Array *arr, size_t *pos)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol);

    b->left = left;
    *pos += 1;

    // Handle unary priority
    if (*pos + 2 < arr->size)
    {
        Token *token = (Token *)(array_get(arr, *pos + 2));

        if (is_unary(token))
        {
            Symbol symbol = array_element_to_symbol(arr, *pos);
            b->right = BinTree(Symbol, &symbol);
            *pos += 2;
            b = parse_unary_operator(b, arr, pos);
        }
        else
            b->right = parse_sub(arr, pos);
    }
    else
        b->right = parse_sub(arr, pos);

    return b;
}

BinTree *parse_paranthesis(Array *arr, size_t *pos)
{
    *pos += 1;
    BinTree *b = parse_sub(arr, pos);
    return b;
}

BinTree *parse_sub(Array *arr, size_t *pos)
{
    Token *token = array_get(arr, *pos);
    Token *old_token = NULL;
    BinTree *b = NULL;

    // Handle opening paranthesis
    if (token->type == PUNCTUATION && token->value == '(' && *pos < arr->size)
    {
        return parse_paranthesis(arr, pos);
    }

    // Initialise the binay tree
    Symbol symbol = array_element_to_symbol(arr, *pos);
    b = BinTree(Symbol, &symbol);

    // Increment the position
    *pos += 1;
    if (*pos >= arr->size)
        return b;
    token = array_get(arr, *pos);

    // Handle closing paranthesis
    if (*pos < arr->size && token->type == PUNCTUATION && token->value == ')')
    {
        *pos += 1;
        return b;
    }

    // Check the validity of the position
    //if (*pos >= arr->size)
    //return b;
    token = array_get(arr, *pos);

    // Get previous operator (for priorities)
    if (*pos > 1)
        old_token = array_get(arr, *pos - 2);
    else
        return b;

    // If old_token is a paranthesis, we don't car about operator priorities
    if (old_token->type == PUNCTUATION && old_token->value == '(')
        if (is_unary(token))
            b = parse_unary_operator(b, arr, pos);
        else
            b = parse_binary_operator(b, arr, pos);
    else
    {
        // The previous punctuation was a concatenation
        if (old_token->type == PUNCTUATION && old_token->value == '.')
        {
            // Only unary operators have priority
            if (is_unary(token))
                b = parse_unary_operator(b, arr, pos);
        }
        else
        {
            // The previous punctuation was an union
            if (old_token->type == PUNCTUATION && old_token->value == '|')
            {
                // Both unary and binary have priority (except the union)
                if (token->type == PUNCTUATION && token->value == '.')
                    b = parse_binary_operator(b, arr, pos);
                else if (is_unary(token))
                    b = parse_unary_operator(b, arr, pos);
            }
        }
    }
    return b;
}

BinTree *parse_symbols(Array *arr)
{
    if (arr == NULL)
        return NULL;

    size_t pos = 0;
    size_t size = arr->size;
    // This value is unused, I don't know what it's for
    // but I have commented it because it produces a warning.
    //Symbol symbol = array_element_to_symbol(arr, pos);
    BinTree *b = parse_sub(arr, &pos);

    while (pos < size)
    {
        Token *token = array_get(arr, pos);
        if (token->type == PUNCTUATION)
        {
            if (is_binary(token))
                b = parse_binary_operator(b, arr, &pos);
            else
            {
                if (is_unary(token))
                    b = parse_unary_operator(b, arr, &pos);
                else if (token->type == PUNCTUATION && token->value == ')')
                    pos += 1;
            }
        }
    }
    return b;
}
