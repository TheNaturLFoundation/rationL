#include "parsing/parsing.h"

#include <stdio.h>

#include "datatypes/array.h"
#include "datatypes/bin_tree.h"
#include "parsing/lexer.h"

BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos, int group, int *max_group);
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos, int group,
                      int *max_group);
BinTree *parse_brackets(Array *arr, size_t *pos, int group, int *max_group);
BinTree *parse_sub(BinTree *b, Array *arr, size_t *pos, int group,
                   int *max_group);

/**
 * Turn a token into a symbol
 * @author Antoine Sicard
 * @date 13/03/2021
 * @return The created symbol
 */
Symbol token_to_symbol(Token *token)
{
    Symbol symbol;

    if (token->type == LITERAL)
    {
        symbol.type = LETTER;
        symbol.value.letter = token->value.letter;
    }
    else if (token->type == PUNCTUATION)
    {
        symbol.type = OPERATOR;
        switch (token->value.letter)
        {
        case '.':
            symbol.value.operator= CONCATENATION;
            break;
        case '|':
            symbol.value.operator= UNION;
            break;
        case '*':
            symbol.value.operator= KLEENE_STAR;
            break;
        case '+':
            symbol.value.operator= EXISTS;
            break;
        case '?':
            symbol.value.operator= MAYBE;
            break;
        }
    }
    else
    {
        symbol.type = CHARACTER_CLASS;
        symbol.value.letters = token->value.letters;
    }
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
    if (token->type == PUNCTUATION)
    {
        switch (token->value.letter)
        {
        case '.':
        case '|':
            return 1;
        default:
            return 0;
        }
    }
    return 0;
}

int is_unary(Token *token)
{
    if(token->type != PUNCTUATION)
        return 0;
    switch (token->value.letter)
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
BinTree *parse_unary(BinTree *left, Array *arr, size_t *pos, int group, int *max_group)
{
    Symbol symbol = array_element_to_symbol(arr, *pos);
    symbol.group = group;
    BinTree *b = BinTree(Symbol, &symbol, .left = NULL, .right = NULL);
    b->left = left;

    *pos += 1;

    // Handle binary priority
    if (*pos < arr->size)
    {
        Token *token = array_get(arr, *pos);
        if (is_binary(token))
            b = parse_binary(b, arr, pos, group, max_group);

    }
    return b;
}

//   . and |    (CONCATENATION and UNION)
BinTree *parse_binary(BinTree *left, Array *arr, size_t *pos, int group,
                      int *max_group)
{
    // Initialise binary tree
    Symbol symbol = array_element_to_symbol(arr, *pos);
    symbol.group = group;
    BinTree *b = BinTree(Symbol, &symbol, .left = left, .right = NULL);

    // Increment position
    *pos += 1;
    Token *token = array_get(arr, *pos);

    // Handle opening brackets
    if (token->type == PUNCTUATION
        && (token->value.letter == '(' || token->value.letter == '{'))
    {
        b->right = parse_brackets(arr, pos, group, max_group);
        return b;
    }

    // Handle next operator
    if (*pos + 1 < arr->size)
    {
        Token *next_token = array_get(arr, *pos + 1);
        // next_token is a PUNCTUATION

        if (next_token->value.letter == '.')
        {
            Symbol s = array_element_to_symbol(arr, *pos);
            s.group = group;
            *pos += 1;
            b->right =
                parse_binary(BinTree(Symbol, &s, .left = NULL, .right = NULL),
                             arr, pos, group, max_group);

            // b->right = parse_sub(NULL, arr, pos, group, max_group);
            return b;
        }
        if (next_token->value.letter == '|')
        {
            Symbol s = array_element_to_symbol(arr, *pos);
            s.group = group;
            b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
            *pos += 1;
            return b; // parse_binary(b, arr, pos, group, max_group);
        }
        if (is_unary(next_token))
        {
            Symbol s = array_element_to_symbol(arr, *pos);
            s.group = group;
            *pos += 1;
            b->right =
                parse_unary(BinTree(Symbol, &s, .left = NULL, .right = NULL),
                            arr, pos, group, max_group);
            return b;
        }
    }
    // If the current position is the last token or next token is closing
    // bracket
    Symbol s = array_element_to_symbol(arr, *pos);
    s.group = group;
    b->right = BinTree(Symbol, &s, .left = NULL, .right = NULL);
    *pos += 1;
    return b;
}

// Handle brackets ( { ) }
BinTree *parse_brackets(Array *arr, size_t *pos, int group, int *max_group)
{
    Token *token = array_get(arr, *pos);

    // Handle groups
    if (token->value.letter == '{')
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

    BinTree *b = parse_sub(NULL, arr, pos, group, max_group);
    token = array_get(arr, *pos);

    // Handle closing brackets
    if (token->type == PUNCTUATION
        && (token->value.letter == ')' || token->value.letter == '}'))
    {
        group -= 1;
        *pos += 1;
        if (*pos < arr->size)
        {
            Token *tok = array_get(arr, *pos);
            if (tok->type == PUNCTUATION && is_unary(tok))
            {
                return parse_unary(b, arr, pos, group, max_group);
            }
            if (tok->type == PUNCTUATION && tok->value.letter == '.')
            {
                return parse_binary(b, arr, pos, group, max_group);
            }
        }

        return b;
    }

    if (token->type == PUNCTUATION
        && (token->value.letter == '|' || token->value.letter == '.'))
    {
        while (token->type == PUNCTUATION
               && (token->value.letter == '|' || token->value.letter == '.'))
        {
            b = parse_sub(b, arr, pos, group, max_group);
            if (*pos < arr->size)
            {
                token = array_get(arr, *pos);

                // Handle closing brackets
                if (token->type == PUNCTUATION
                    && (token->value.letter == ')'
                        || token->value.letter == '}'))
                {
                    group -= 1;
                    *pos += 1;
                    if (*pos < arr->size)
                    {
                        Token *tok = array_get(arr, *pos);
                        if (tok->type == PUNCTUATION && is_unary(tok))
                        {
                            return parse_unary(b, arr, pos, group, max_group);
                        }
                        if (tok->type == PUNCTUATION
                            && tok->value.letter == '.')
                        {
                            return parse_binary(b, arr, pos, group, max_group);
                        }
                    }

                    return b;
                }
            }
        }
        return b;
    }

    // if (token->type == PUNCTUATION && is_unary(token))
    //{
    return parse_unary(b, arr, pos, group, max_group);
    //}
}

BinTree *parse_sub(BinTree *b, Array *arr, size_t *pos, int group,
                   int *max_group)
{
    // Check position
    if (*pos >= arr->size)
        return b;

    Token *token = array_get(arr, *pos);

    // Handle closing brackets
    if (token->type == PUNCTUATION
        && (token->value.letter == ')' || token->value.letter == '}'))
    {
        return b;
    }

    // Handle opening brackets
    if (token->type == PUNCTUATION
        && (token->value.letter == '(' || token->value.letter == '{'))
    {
        if (token->value.letter == '{')
            return parse_brackets(arr, pos, group, max_group);
        else
            return parse_brackets(arr, pos, group, max_group);
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
    if (token->type == PUNCTUATION
        && (token->value.letter == ')' || token->value.letter == '}'))
        return b;

    // Handle binary operator
    if (is_binary(token))
    {
        return parse_binary(b, arr, pos, group, max_group);
    }
    // Handle unary operator
    return parse_unary(b, arr, pos, group, max_group);
}

BinTree *parse_symbols(Array *arr)
{
    if (arr == NULL)
        return NULL;

    size_t pos = 0;
    int group = 0;
    int max_group = 0;
    BinTree *b = NULL;

    while (pos < arr->size)
    {
        Token *token = array_get(arr, pos);
        if (token->type == PUNCTUATION
            && (token->value.letter == '}' || token->value.letter == ')'))
            pos += 1;
        b = parse_sub(b, arr, &pos, group, &max_group);
    }
    return b;
}
