#include "parsing/parsing.h"

#include <stdio.h>

#include "datatypes/array.h"
#include "datatypes/bin_tree.h"
#include "parsing/lexer.h"

BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos, int group);
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos, int group, int *max_group, int *curr_group);
BinTree *parse_brackets(Array *arr, size_t *pos, int group, int *max_group, int *curr_group);
BinTree *parse_sub(BinTree *b, Array *arr, size_t *pos, int group, int *max_group, int *curr_group);

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
BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos, int group)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    symbol.group = group;
    BinTree *b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
    b->left = left;

    *pos += 1;
    return b;
}

//   . and |    (CONCATENATION and UNION)
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos, int group, int *max_group, int *curr_group)
{
    // Initialise binary tree
    Symbol symbol = array_element_to_symbol(arr, *pos);
    symbol.group = group;
    BinTree *b = BinTree(Symbol, &symbol, .left = left, .right = NULL);

    // Save first token
    Token *prev_token = array_get(arr, *pos);

    // Increment position
    *pos += 1;
    Token *token = array_get(arr, *pos);

    // Handle opening brackets
    if (token->type == PUNCTUATION && (token->value == '(' || token->value == '{'))
    {
        b->right = parse_brackets(arr, pos, group, max_group, curr_group);
        return b;
    }

    // Handle next operator
    if (*pos+1 < arr->size)
    {
        Token *next_token = array_get(arr, *pos+1);
        // next_token is a PUNCTUATION

        if (next_token->value == '.')
        {
            b->right = parse_sub(NULL, arr, pos, group, max_group, curr_group);
            return b;
        }
        if (next_token->value == '|')
        {
            Symbol s = array_element_to_symbol(arr, *pos);
            s.group = group;
            b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
            *pos += 1;
            return parse_binary(b, arr, pos, group, max_group, curr_group);
        }
        if (is_unary(next_token))
        {
            Symbol s = array_element_to_symbol(arr, *pos);
            s.group = group;
            *pos += 1;
            b->right = parse_unary(BinTree(Symbol, &s, .left = NULL, .right = NULL), arr, pos, group);
            return b;
        }
    }
    // If the current position is the last token or next token is closing bracket
    Symbol s = array_element_to_symbol(arr, *pos);
    s.group = group;
    b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
    *pos += 1;
    return b;
}

// Handle brackets ( { ) }
BinTree *parse_brackets(Array *arr, size_t *pos, int group, int *max_group, int *curr_group)
{
    Token *token = array_get(arr, *pos);
    *curr_group = group;

    // Handle groups
    if (token->value == '{')
    {
        if (group + 1 <= *max_group)
        {
            *max_group += 1;
            group = *max_group;
        }
        else
        {
            *max_group += 1;
            group += 1;
        }
    }

    // Increment position
    *pos += 1;
    token = array_get(arr, *pos);

    BinTree *b = parse_sub(NULL, arr, pos, group, max_group, curr_group);
    token = array_get(arr, *pos);

    // Handle closing brackets
    if (token->type == PUNCTUATION && (token->value == ')' || token->value == '}'))
    {
        group -= 1;
        *pos += 1;
        if (*pos < arr->size)
        {
            Token *tok = array_get(arr, *pos);
            if (tok->type == PUNCTUATION && is_unary(tok))
            {
                return parse_unary(b, arr, pos, group);
            }
        }

        return b;
    }

    if (token->type == PUNCTUATION && (token->value == '|' || token->value == '.'))
    {
        while (token->type == PUNCTUATION && (token->value == '|' || token->value == '.'))
        {
            //b = parse_binary(b, arr, pos, group, max_group, curr_group);
            b = parse_sub(b, arr, pos, group, max_group, curr_group);
            if (*pos < arr->size)
                token = array_get(arr, *pos);
        }
        return b;
    }

    if (token->type == PUNCTUATION && is_unary(token))
    {
        return parse_unary(b, arr, pos, group);
    }
}

BinTree *parse_sub(BinTree *b, Array *arr, size_t *pos, int group, int *max_group, int *curr_group)
{
    // Check position
    if (*pos >= arr->size)
        return b;

    Token *token = array_get(arr, *pos);

    // Handle closing brackets
    if (token->type == PUNCTUATION && (token->value == ')' || token->value == '}'))
    {
        return b;
    }

    // Handle opening brackets
    if (token->type == PUNCTUATION && (token->value == '(' || token->value == '{'))
    {
        if (token->value == '{')
            return parse_brackets(arr, pos, group, max_group, curr_group);
        else
            return parse_brackets(arr, pos, group, max_group, curr_group);
    }

    // Initialise the binary tree
    if (b == NULL)
    {
        Symbol symbol = array_element_to_symbol(arr, *pos);
        symbol.group = group;
        b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);

        // Increment the position
        *pos += 1;
        if (*pos >= arr->size)
            return b;
        token = array_get(arr, *pos);
    }

    // Handle closing brackets
    if (token->type == PUNCTUATION && (token->value == ')' || token->value == '}'))
        return b;

    // Handle binary operator
    if (is_binary(token))
    {
        return parse_binary(b, arr, pos, group, max_group, curr_group);
    }
    // Handle unary operator
    return parse_unary(b, arr, pos, group);
}

BinTree *parse_symbols(Array *arr)
{
    if (arr == NULL)
        return NULL;

    size_t pos = 0;
    int group = 0;
    int max_group = 0;
    int curr_group = 0;
    BinTree *b = NULL;

    //b = parse_sub(b, arr, &pos, group, &max_group, &curr_group);

    while (pos < arr->size)
    {
        Token *token = array_get(arr, pos);
        if (token->type == PUNCTUATION && (token->value == '}' || token->value == ')'))
            pos += 1;
        b = parse_sub(b, arr, &pos, group, &max_group, &curr_group);

    }
    return b;
}
