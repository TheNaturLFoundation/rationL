#include "datatypes/bin_tree.h"
#include "parsing.h"
#include "lexer.h"

#include <stdio.h>

Symbol token_to_symbol(Token token)
{

  Symbol symbol;
  SymbolType symbolType;
  SymbolValue value;

  if (token.type == LITERAL)
    {
      symbolType = LETTER;
      symbol.type = symbolType;
      value.letter = token.value;
    }
  else
    {
      symbolType = OPERATOR;
      symbol.type = symbolType;
      switch(token.value)
        {
        case '.':
          value.operator = CONCATENATION;
          break;
        case '|':
          value.operator = UNION;
          break;
        case '*':
          value.operator = KLEENE_STAR;
          break;
        case '+':
          value.operator = EXISTS;
          break;
        case '?':
          value.operator = MAYBE;
          break;
        }
    }
  symbol.value = value;
  return symbol;
}

Symbol array_element_to_symbol(Array *arr, size_t pos)
{
  Token token = *(Token *)(array_get(arr, pos));
  Symbol symbol = token_to_symbol(token);
  return symbol;
}

BinTree *parse_sub(Array *arr, size_t *pos, Token old_token)
  {
    Symbol symbol = array_element_to_symbol(arr, *pos);

    Token tok = *(Token *)(array_get(arr, *pos));

    if(tok.value == '(')
      {
        *pos+=1;
        return parse_sub(arr, pos, tok);
      }

    BinTree *b = BinTree(Symbol, &symbol);

    if (old_token.value == '(')
      {
        *pos+=1;
        tok = *(Token *)(array_get(arr, *pos));
        switch(tok.value)
          {
          case '.':
          case '|':
            b = parse_union_or_concatenation(b, arr, pos);
            break;
          case '*':
          case '+':
          case '?':
            b = parse_star_exists_or_maybe(b, arr, pos);
            break;
          }
      }

    *pos += 1;
    if (*pos >= arr->size)
      return b;

    tok = *(Token *)(array_get(arr, *pos));

    if(tok.value == ')')
      {
        *pos += 1;
        return b;
      }

    if(old_token.value == '.')
      {
        switch(tok.value)
          {
          case '*':
          case '+':
          case '?':
            b = parse_star_exists_or_maybe(b, arr, pos);
            break;
          }
      }
    else
      {
        if(old_token.value == '|')
          {
            switch(tok.value)
              {
              case '.':
                b = parse_union_or_concatenation(b, arr, pos);
                break;
              case '*':
              case '+':
              case '?':
                b = parse_star_exists_or_maybe(b, arr, pos);
                break;
              }
          }
      }
    return b;
  }

BinTree *parse_star_exists_or_maybe(BinTree *left, Array *arr, size_t *pos)
  {
    Symbol symbol = array_element_to_symbol(arr, *pos);
    BinTree *b = BinTree(Symbol, &symbol);

    b->left = left;
    *pos+=1;

    return b;
  }

BinTree *parse_union_or_concatenation(BinTree *left, Array *arr, size_t *pos)
{
  Symbol symbol = array_element_to_symbol(arr, *pos);
  Token token = *(Token *)(array_get(arr, *pos));
  BinTree *b = BinTree(Symbol, &symbol);

  b->left = left;
  *pos += 1;

  b->right = parse_sub(arr, pos, token);

  return b;
}


BinTree *parse_symbols(Array *arr)
{
  if (arr == NULL)
    return NULL;

  BinTree *b = NULL;
  size_t pos = 0;

  if (pos < arr->size)
    {
      Token token = *(Token *)array_get(arr, pos);
      if (token.value == '(')
        {
          b = parse_sub(arr, &pos, token);
        }
      else
        {
          Symbol symbol = array_element_to_symbol(arr, pos);
          b = BinTree(Symbol, &symbol);
          pos+=1;
        }
    }

  while (pos < arr->size)
    {
      Token token = *(Token *)array_get(arr, pos);
      if(token.type == PUNCTUATION)
        {
          switch(token.value)
            {
            case '.':
            case '|':
              b = parse_union_or_concatenation(b, arr, &pos);
              break;
            case '*':
            case '+':
            case '?':
              b = parse_star_exists_or_maybe(b, arr, &pos);
              break;
            }
        }
    }

  return b;
}
